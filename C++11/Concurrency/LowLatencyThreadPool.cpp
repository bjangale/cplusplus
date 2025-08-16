#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>
#include <future>
#include <type_traits>
#include <new>
#include <cassert>
#include <chrono>
#include <iostream>
#include <functional>

static inline void cpu_relax() noexcept { /* best-effort no-op */ }

#ifndef ULLTP_CACHELINE
#define ULLTP_CACHELINE 64
#endif

struct alignas(ULLTP_CACHELINE) CachelinePad { char pad[ULLTP_CACHELINE]; };

// --------------------------- Job --------------------------------
struct Job {
    using Fn = void(*)(void*);
    Fn fn{nullptr};
    void* data{nullptr};
    void (*deleter)(void*){nullptr}; // optional (for submit path)

    void operator()() noexcept {
        Fn f = fn;
        void* d = data;
        if (f) f(d);
        if (deleter) deleter(d);
    }
};

// ----------------- Bounded MPMC queue (Vyukov) -------------------
class MPMCBoundedQueue {
public:
    explicit MPMCBoundedQueue(size_t capacity_pow2)
    : capacity_(round_up_pow2(capacity_pow2)), mask_(capacity_ - 1),
      buffer_(static_cast<Cell*>(::operator new[](capacity_ * sizeof(Cell), std::align_val_t(ULLTP_CACHELINE))))
    {
        for (size_t i = 0; i < capacity_; ++i) {
            new (&buffer_[i]) Cell();
            buffer_[i].seq.store(static_cast<uint64_t>(i), std::memory_order_relaxed);
        }
        head_.store(0, std::memory_order_relaxed);
        tail_.store(0, std::memory_order_relaxed);
    }

    ~MPMCBoundedQueue() {
        for (size_t i = 0; i < capacity_; ++i) buffer_[i].~Cell();
        ::operator delete[](buffer_, std::align_val_t(ULLTP_CACHELINE));
    }

    bool enqueue(const Job& j) noexcept {
        Cell* cell;
        uint64_t pos = tail_.load(std::memory_order_relaxed);
        for (;;) {
            cell = &buffer_[pos & mask_];
            uint64_t seq = cell->seq.load(std::memory_order_acquire);
            intptr_t dif = (intptr_t)seq - (intptr_t)pos;
            if (dif == 0) {
                if (tail_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                    break;
            } else if (dif < 0) {
                return false; // full
            } else {
                pos = tail_.load(std::memory_order_relaxed);
            }
        }
        cell->job = j;
        cell->seq.store(pos + 1, std::memory_order_release);
        return true;
    }

    bool dequeue(Job& out) noexcept {
        Cell* cell;
        uint64_t pos = head_.load(std::memory_order_relaxed);
        for (;;) {
            cell = &buffer_[pos & mask_];
            uint64_t seq = cell->seq.load(std::memory_order_acquire);
            intptr_t dif = (intptr_t)seq - (intptr_t)(pos + 1);
            if (dif == 0) {
                if (head_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                    break;
            } else if (dif < 0) {
                return false; // empty
            } else {
                pos = head_.load(std::memory_order_relaxed);
            }
        }
        out = cell->job;
        cell->seq.store(pos + mask_ + 1, std::memory_order_release);
        return true;
    }

    size_t capacity() const noexcept { return capacity_; }

private:
    struct alignas(ULLTP_CACHELINE) Cell {
        std::atomic<uint64_t> seq;
        Job job;
    };

    static size_t round_up_pow2(size_t x) {
        if (x < 2) return 2;
        --x;
        for (size_t i = 1; i < sizeof(size_t) * 8; i <<= 1) x |= x >> i;
        return x + 1;
    }

    CachelinePad pad0_;
    const size_t capacity_;
    const size_t mask_;
    Cell* buffer_;
    CachelinePad pad1_;
    std::atomic<uint64_t> head_{0};
    CachelinePad pad2_;
    std::atomic<uint64_t> tail_{0};
    CachelinePad pad3_;
};

// ------------------------ Thread Pool ----------------------------
class LowLatencyThreadPool {
public:
    LowLatencyThreadPool(unsigned threads, size_t queue_capacity_pow2 = 1024,
                         unsigned spin_loops = 256) 
    : queue_(queue_capacity_pow2), spin_loops_(spin_loops), stop_(false)
    {
        if (threads == 0) threads = std::max(1u, std::thread::hardware_concurrency());
        workers_.reserve(threads);
        for (unsigned i = 0; i < threads; ++i) {
            workers_.emplace_back([this] { this->worker_loop(); });
        }
    }

    ~LowLatencyThreadPool() {
        shutdown();
    }

    // --- Ultra-low-latency: zero-allocation enqueue ---
    // You own 'data' lifetime; optionally supply 'deleter' to clean after run.
    bool enqueue_raw(Job::Fn fn, void* data, void(*deleter)(void*) = nullptr) noexcept {
        Job j{fn, data, deleter};
        return queue_.enqueue(j);
    }

    // Convenience submit with future (may allocate). Prefer enqueue_raw for HFT path.
    template <class F, class... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>
    {
        using R = std::invoke_result_t<F, Args...>;
        using Packaged = std::packaged_task<R()>;

        // Create heap package; pool will delete via deleter.
        auto bound = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto* pkg = new Packaged(std::move(bound));
        std::future<R> fut = pkg->get_future();

        auto run_pkg = [](void* p) noexcept {
            auto* pk = static_cast<Packaged*>(p);
            (*pk)();
        };
        auto del_pkg = [](void* p) noexcept {
            delete static_cast<Packaged*>(p);
        };

        // Busy-wait a little to preserve latency rather than blocking.
        for (unsigned i = 0; i < spin_loops_; ++i) {
            if (queue_.enqueue(Job{run_pkg, pkg, del_pkg})) return fut;
            cpu_relax();
        }

        // Final try; if still full, block minimally with a short yield loop.
        while (!queue_.enqueue(Job{run_pkg, pkg, del_pkg})) {
            std::this_thread::yield();
        }
        return fut;
    }

    // Drains and joins. Safe to call multiple times.
    void shutdown() noexcept {
        bool expected = false;
        if (!stop_.compare_exchange_strong(expected, true, std::memory_order_relaxed))
            return;
        // Wake workers by injecting no-ops if needed (optional).
        for (size_t i = 0; i < workers_.size(); ++i) {
            // Best effort: push wakeups; ignore if full.
            queue_.enqueue(Job{nullptr, nullptr, nullptr});
        }
        for (auto& t : workers_) if (t.joinable()) t.join();
        workers_.clear();
    }

    // Optional: set a soft spin loop count for both submit & workers.
    void set_spin_loops(unsigned loops) noexcept { spin_loops_ = loops; }

    size_t queue_capacity() const noexcept { return queue_.capacity(); }
    unsigned size() const noexcept { return static_cast<unsigned>(workers_.size()); }

private:
    void worker_loop() noexcept {
        Job j;
        unsigned spins = 0;
        while (!stop_.load(std::memory_order_relaxed)) {
            if (queue_.dequeue(j)) {
                spins = 0;
                if (j.fn) j(); // null job used as wake signal on shutdown
                continue;
            }
            // Spin a bit for ultra-low latency handoff
            if (spins < spin_loops_) {
                ++spins;
                cpu_relax();
            } else {
                // Back off to avoid burning a full core indefinitely
                std::this_thread::yield();
            }
        }
        // Drain remaining work on shutdown
        while (queue_.dequeue(j)) {
            if (j.fn) j();
        }
    }

    MPMCBoundedQueue queue_;
    std::vector<std::thread> workers_;
    std::atomic<unsigned> spin_loops_;
    std::atomic<bool> stop_;
};

// --------------------- Example raw helpers -----------------------
template <typename F>
struct RawThunk {
    F fn;
    static void run(void* p) noexcept {
        F* fp = static_cast<F*>(p);
        (*fp)();
    }
    static void del(void* p) noexcept {
        delete static_cast<F*>(p);
    }
};

// Helper to enqueue a callable without future, with heap-owned callable.
// Prefer this only if you don't want to manage your own void* payload.
template <class Pool, class F>
inline bool enqueue_callable(Pool& pool, F&& f) {
    using Fn = std::decay_t<F>;
    auto* holder = new Fn(std::forward<F>(f));
    return pool.enqueue_raw(&RawThunk<Fn>::run, holder, &RawThunk<Fn>::del);
}

int main() {
    LowLatencyThreadPool pool(4, 1024, /*spin_loops=*/512);

    // Ultra-low-latency path: raw function pointer + payload
    struct Payload { int x, y; };
    auto work = [](void* p) noexcept {
        auto* pl = static_cast<Payload*>(p);
        int s = pl->x + pl->y; // do your thing
        // No prints in hot paths—this is just a demo
    };
    Payload* p = new Payload{1,2};
    pool.enqueue_raw(work, p, [](void* q){ delete static_cast<Payload*>(q); });

    // Convenience path with future (may allocate)
    auto fut = pool.submit([](int a, int b){ return a + b; }, 3, 4);
    std::cout << "sum=" << fut.get() << "\n";

    pool.shutdown();
}
