#include <iostream>
#include <stack>
#include <memory>

template <typename T>
class MemoryPool {
public:
    explicit MemoryPool(size_t poolSize)
        : poolSize(poolSize), memoryBlock(new char[poolSize * sizeof(T)]) {
        initializeStack();
    }

    ~MemoryPool() {
        delete[] memoryBlock;
    }

    // Allocate an object from the pool
    template <typename... Args>
    T* allocate(Args&&... args) {
        if (freeStack.empty()) {
            throw std::bad_alloc();  // Pool exhausted
        }

        // Get a free block from the stack
        T* allocatedObject = freeStack.top();
        freeStack.pop();

        // Construct the object in-place
        return new (allocatedObject) T(std::forward<Args>(args)...);
    }

    // Deallocate an object, returning it to the pool
    void deallocate(T* object) {
        if (object) {
            object->~T();  // Call destructor explicitly
            freeStack.push(object);  // Return memory block to the stack
        }
    }

private:
    size_t poolSize;
    char* memoryBlock;
    std::stack<T*> freeStack;

    // Initializes the stack with pointers to each block in the pool
    void initializeStack() {
        for (size_t i = 0; i < poolSize; ++i) {
            T* ptr = reinterpret_cast<T*>(memoryBlock + i * sizeof(T));
            freeStack.push(ptr);
        }
    }
};


