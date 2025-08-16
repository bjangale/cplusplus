#include <gtest/gtest.h>
#include <string>
#include <thread>

#include "lockfreequeue.hpp"
constexpr size_t queue_capacity = 4;

class LockFreeQueueTest : public ::testing::Test {
protected:
 LockFreeQueueTest() : queue(queue_capacity)
 {

 }

    LockFreeQueue<std::string> queue;
};

// Test case to check if enqueueing works correctly
TEST_F(LockFreeQueueTest, Enqueue) {
    EXPECT_TRUE(queue.enqueue("1"));
    EXPECT_TRUE(queue.enqueue("2"));
    EXPECT_TRUE(queue.enqueue("3"));
 
    // Trying to enqueue when the queue is full should return false
    EXPECT_FALSE(queue.enqueue("4"));
}

// Test case to check if dequeueing works correctly
TEST_F(LockFreeQueueTest, Dequeue) {
    std::string item;

    // Enqueue items to the queue first
    queue.enqueue("10");
    queue.enqueue("20");

    // Dequeue items and check values
    EXPECT_TRUE(queue.dequeue(item));
    EXPECT_EQ(item, std::string("10"));

    EXPECT_TRUE(queue.dequeue(item));
    EXPECT_EQ(item, std::string("20"));

    // Dequeue from an empty queue should return false
    EXPECT_FALSE(queue.dequeue(item));
}

// Test case to check enqueue-dequeue combination
TEST_F(LockFreeQueueTest, EnqueueDequeueCombination) {
    std::string item;

    // Enqueue items
    EXPECT_TRUE(queue.enqueue("100"));
    EXPECT_TRUE(queue.enqueue("200"));
    EXPECT_TRUE(queue.enqueue("300"));

    // Dequeue one item
    EXPECT_TRUE(queue.dequeue(item));
    EXPECT_EQ(item, std::string("100"));

    // Enqueue another item
    EXPECT_TRUE(queue.enqueue("400"));

    // Dequeue remaining items
    EXPECT_TRUE(queue.dequeue(item));
    EXPECT_EQ(item, std::string("200"));

    EXPECT_TRUE(queue.dequeue(item));
    EXPECT_EQ(item, std::string("300"));

    EXPECT_TRUE(queue.dequeue(item));
    EXPECT_EQ(item, std::string("400"));

    // Queue should be empty now
    EXPECT_FALSE(queue.dequeue(item));
}

TEST_F(LockFreeQueueTest, EnqueueDequeueCombinationParallel) {
    constexpr int count = 1000;
    std::string testString("This is my test string ");
    // Producer
    std::thread producer([&]()
                         {
        for (int i = 1; i <= count; ++i) {
            std::string str = testString + std::to_string(i);
            while (!queue.enqueue(str)) {
                // Retry until the item is enqueued
            }
        } });

    for (int i = 1; i <= count; ++i)
    {
        std::string item;
        while (!queue.dequeue(item))
        {
            // Retry until the item is dequeued
        }
         std::string expectedString = testString + std::to_string(i);
        EXPECT_EQ(expectedString,item);
    }

    producer.join();
}

TEST_F(LockFreeQueueTest, EnqueueDequeueMoveCombinationParallel) {
    constexpr int count = 1000;
    std::string testString("This is my test string ");
    // Producer
    std::thread producer([&]()
                         {
        for (int i = 1; i <= count; ++i) {
            std::string str = testString + std::to_string(i);
            while (!queue.enqueue(std::move(str))) {
                // Retry until the item is enqueued
            }
        } });

    for (int i = 1; i <= count; ++i)
    {
        std::string item;
        while (!queue.dequeue(item))
        {
            // Retry until the item is dequeued
        }
         std::string expectedString = testString + std::to_string(i);
        EXPECT_EQ(expectedString,item);
    }

    producer.join();
}