/**
 * Simple link list with dummy node. 
 */
#include <iostream>
#include <memory>
#include <mutex>

struct EmptyQueue : std::exception
{
    const char *what()
    {
        return "Queue is empty";
    }
};

template <typename T>
class SimpleQueueUsingLinkList
{
private:
    struct node
    {
        std::unique_ptr<node> next;
        std::shared_ptr<T> data;
    };

    std::unique_ptr<node> head;
    node *tail;

public:
    /**
     * Dummy node is to avoid head and tail lock at same time, when only one node
     * is present in the list. This ensures that there is always once node in the
     * queue to separate the node being accessed at the head from that being accessed
     * at the tail.
     */
    SimpleQueueUsingLinkList() : head(new node), tail(head.get()) {}
    SimpleQueueUsingLinkList(const SimpleQueueUsingLinkList &) = delete;
    SimpleQueueUsingLinkList &operator=(const SimpleQueueUsingLinkList &) = delete;

    std::shared_ptr<T> try_pop()
    {
        // For empty queue, head and tail now point to dummy node
        // rather than NULL
        if (head.get() == tail)
            throw EmptyQueue();

        std::shared_ptr<T> const res(head->data);
        // Old_head will delete itself once it goes out of scope.
        // std::unique_ptr<node> oldHead = std::move(head);
        // head = std::move(oldHead->next);
        // This will also delete old head 
        head = std::move(head->next);
        return res;
    }

    void push(T newVal)
    {
        auto newData = std::make_shared<T>(std::move(newVal));
        //newNode created is going to be new dummy node.
        std::unique_ptr<node> newNode(new node);
        tail->data = newData;
        node *const newTail = newNode.get();
        tail->next = std::move(newNode);
        tail = newTail;
    }
};

int main()
{
    SimpleQueueUsingLinkList<int> queue;

    queue.push(10);
    queue.push(19);

    std::cout << *queue.try_pop() << std::endl;
    std::cout << *queue.try_pop() << std::endl;
}