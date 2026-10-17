#ifndef STACKQUEUE_H
#define STACKQUEUE_H
#include <iostream>
using namespace std;

// Stack: last in, first out
class SimpleStack
{
    int items[100];
    int top;

public:
    SimpleStack()
    {
        top = -1;
    }

    bool push(int x)
    {
        if (top == 99)
        {
            return false;
        }
        items[++top] = x;
        return true;
    }

    bool pop(int &x)
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
    bool peekFromTop(int k, int &x)
    {
        if (k < 0 || k > top)
        {
            return false;
        }
        x = items[top - k];
        return true;
    }
};

// Queue: first in, first out (circular array)
class SimpleQueue
{
    int items[100];
    int front;
    int count;

public:
    SimpleQueue()
    {
        front = 0;
        count = 0;
    }

    bool enqueue(int x)
    {
        if (count == 100)
        {
            return false;
        }
        items[(front + count) % 100] = x;
        count++;
        return true;
    }

    bool dequeue(int &x)
    {
        if (count == 0)
        {
            return false;
        }
        x = items[front];
        front = (front + 1) % 100;
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
    bool peekAt(int k, int &x)
    {
        if (k < 0 || k >= count)
        {
            return false;
        }
        x = items[(front + k) % 100];
        return true;
    }
};

#endif
