#pragma once

#include <atomic>
#include <memory>
#include <iostream>

#include <type_traits>

// Type trait to check if a type is both copyable and movable
template <typename T>
struct is_copyable_and_movable
    : std::integral_constant<bool,
        std::is_copy_constructible<T>::value && std::is_move_constructible<T>::value> {};

// Helper variable template (C++17 and above)
template <typename T>
inline constexpr bool is_copyable_and_movable_v = is_copyable_and_movable<T>::value;

template <typename T>
class LockFreeQueue {
public:

explicit LockFreeQueue(size_t _capacity);

    // Enqueue with move semantics
    bool enqueue(T&& item) noexcept;

        // Enqueue with move semantics
    bool enqueue(const T& item) noexcept ;
    // Dequeue with move semantics
    bool dequeue(T& item) noexcept;

    LockFreeQueue(const LockFreeQueue<T>&) = delete;
    LockFreeQueue(const LockFreeQueue<T>&&) = delete;
private:
    size_t increment(size_t idx) const; 

    std::unique_ptr<T[]> buffer;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
    size_t capacity;
};


template <typename T>
LockFreeQueue<T>::LockFreeQueue(size_t _capacity)
    : capacity(_capacity), head(0), tail(0)
{
    static_assert(is_copyable_and_movable_v<T>,"Queue object should be copy and move constructible");

    buffer = std::unique_ptr<T[]>(new T[capacity]);
}

// Enqueue with move semantics
template <typename T>
bool LockFreeQueue<T>::enqueue(T &&item) noexcept
{
    size_t currentTail = tail.load(std::memory_order_relaxed);
    size_t nextTail = increment(currentTail);

    if (nextTail == head.load(std::memory_order_acquire))
    {
        // Queue is full
        return false;
    }

    buffer[currentTail] = std::move(item); // Move the item
    tail.store(nextTail, std::memory_order_release);
    return true;
}

// Enqueue with move semantics
template <typename T>
bool LockFreeQueue<T>::enqueue(const T &item) noexcept
{
    size_t currentTail = tail.load(std::memory_order_relaxed);
    size_t nextTail = increment(currentTail);

    if (nextTail == head.load(std::memory_order_acquire))
    {
        // Queue is full
        return false;
    }

    buffer[currentTail] = item; // copy the item
    tail.store(nextTail, std::memory_order_release);
    return true;
}

// Dequeue with move semantics
template <typename T>
bool LockFreeQueue<T>::dequeue(T &item) noexcept
{
    size_t currentHead = head.load(std::memory_order_relaxed);

    if (currentHead == tail.load(std::memory_order_acquire))
    {
        // Queue is empty
        return false;
    }

    item = std::move(buffer[currentHead]); // Move the item out
    head.store(increment(currentHead), std::memory_order_release);
    return true;
}

template <typename T>
size_t LockFreeQueue<T>::increment(size_t idx) const
{
   //  return (idx + 1) % capacity;
    return (idx + 1) & (capacity - 1);
}