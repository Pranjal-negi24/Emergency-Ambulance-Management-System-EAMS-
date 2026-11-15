#ifndef STACKQUEUE_H
#define STACKQUEUE_H
#include <iostream>
using namespace std;

// Stack: last in, first out. Grows automatically when it becomes full.
template <class T>
class SimpleStack
{
    T *items;
    int capacity;
    int top;

public:
    SimpleStack()
    {
        capacity = 16;
        top = -1;
        items = new T[capacity];
    }

    ~SimpleStack()
    {
        delete[] items;
    }

    SimpleStack(const SimpleStack &) = delete;
    SimpleStack &operator=(const SimpleStack &) = delete;

    void push(const T &x)
    {
        if (top == capacity - 1)
        {
            capacity = capacity * 2;
            T *bigger = new T[capacity];
            for (int i = 0; i <= top; i++)
            {
                bigger[i] = items[i];
            }
            delete[] items;
            items = bigger;
        }
        items[++top] = x;
    }

    bool pop(T &x)
    {
        if (top == -1)
        {
            return false;
        }
        x = items[top--];
        return true;
    }

    bool empty()
    {
        return top == -1;
    }

    int size()
    {
        return top + 1;
    }

    // k = 0 is the top (most recent) item; does not remove anything
    bool peekFromTop(int k, T &x)
    {
        if (k < 0 || k > top)
        {
            return false;
        }
        x = items[top - k];
        return true;
    }

    // k = 0 is the bottom (oldest) item; does not remove anything
    bool peekFromBottom(int k, T &x)
    {
        if (k < 0 || k > top)
        {
            return false;
        }
        x = items[k];
        return true;
    }
};

// Queue: first in, first out (circular array). Grows automatically when it becomes full.
template <class T>
class SimpleQueue
{
    T *items;
    int capacity;
    int front;
    int count;

public:
    SimpleQueue()
    {
        capacity = 16;
        front = 0;
        count = 0;
        items = new T[capacity];
    }

    ~SimpleQueue()
    {
        delete[] items;
    }

    SimpleQueue(const SimpleQueue &) = delete;
    SimpleQueue &operator=(const SimpleQueue &) = delete;

    void enqueue(const T &x)
    {
        if (count == capacity)
        {
            T *bigger = new T[capacity * 2];
            for (int i = 0; i < count; i++)
            {
                bigger[i] = items[(front + i) % capacity];
            }
            delete[] items;
            items = bigger;
            capacity = capacity * 2;
            front = 0;
        }
        items[(front + count) % capacity] = x;
        count++;
    }

    bool dequeue(T &x)
    {
        if (count == 0)
        {
            return false;
        }
        x = items[front];
        front = (front + 1) % capacity;
        count--;
        return true;
    }

    bool empty()
    {
        return count == 0;
    }

    int size()
    {
        return count;
    }

    // k = 0 is the front (oldest) item; does not remove anything
    bool peekAt(int k, T &x)
    {
        if (k < 0 || k >= count)
        {
            return false;
        }
        x = items[(front + k) % capacity];
        return true;
    }
};

#endif
