#include <gtest/gtest.h>
#include "MemoryPool.hpp" 

// Test class to use with MemoryPool
class TestObject {
public:
    TestObject(int x, double y) : x(x), y(y) {}
    int x;
    double y;
};

// Test fixture for MemoryPool
class MemoryPoolTest : public ::testing::Test {
protected:
    static constexpr size_t poolSize = 3;
    MemoryPool<TestObject> pool{poolSize};

    void SetUp() override {
        // Initialization before each test
    }

    void TearDown() override {
        // Cleanup after each test
    }
};

// Test for successful allocation
TEST_F(MemoryPoolTest, AllocationWorks) {
    TestObject* obj1 = pool.allocate(1, 1.5);
    ASSERT_NE(obj1, nullptr);
    EXPECT_EQ(obj1->x, 1);
    EXPECT_DOUBLE_EQ(obj1->y, 1.5);

    TestObject* obj2 = pool.allocate(2, 2.5);
    ASSERT_NE(obj2, nullptr);
    EXPECT_EQ(obj2->x, 2);
    EXPECT_DOUBLE_EQ(obj2->y, 2.5);

    pool.deallocate(obj1);
    pool.deallocate(obj2);
}

// Test for deallocation and reusability of the memory pool
TEST_F(MemoryPoolTest, DeallocationReusesMemory) {
    TestObject* obj1 = pool.allocate(1, 1.5);
    ASSERT_NE(obj1, nullptr);

    pool.deallocate(obj1);

    // Reallocate and check if the pool reuses memory
    TestObject* obj2 = pool.allocate(2, 2.5);
    ASSERT_NE(obj2, nullptr);
    EXPECT_EQ(obj2->x, 2);
    EXPECT_DOUBLE_EQ(obj2->y, 2.5);

    pool.deallocate(obj2);
}

// Test for pool exhaustion
TEST_F(MemoryPoolTest, PoolExhaustionThrows) {
    TestObject* obj1 = pool.allocate(1, 1.5);
    TestObject* obj2 = pool.allocate(2, 2.5);
    TestObject* obj3 = pool.allocate(3, 3.5);

    // Expect std::bad_alloc if we exceed pool capacity
    EXPECT_THROW(pool.allocate(4, 4.5), std::bad_alloc);

    // Cleanup
    pool.deallocate(obj1);
    pool.deallocate(obj2);
    pool.deallocate(obj3);
}

// Test for valid deallocation of null pointer
TEST_F(MemoryPoolTest, DeallocateNull) {
    TestObject* obj = nullptr;
    EXPECT_NO_THROW(pool.deallocate(obj));
}
