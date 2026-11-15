#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

// Dynamic array: starts small and doubles its size when it becomes full,
// so the system has no fixed limit on patients, ambulances or hospitals.
template <class T>
class DynamicArray
{
    T *data;
    int count;
    int capacity;

public:
    DynamicArray()
    {
        capacity = 8;
        count = 0;
        data = new T[capacity];
    }

    ~DynamicArray()
    {
        delete[] data;
    }

    DynamicArray(const DynamicArray &) = delete;
    DynamicArray &operator=(const DynamicArray &) = delete;

    void add(const T &item)
    {
        if (count == capacity)
        {
            capacity = capacity * 2;
            T *bigger = new T[capacity];
            for (int i = 0; i < count; i++)
            {
                bigger[i] = data[i];
            }
            delete[] data;
            data = bigger;
        }
        data[count] = item;
        count++;
    }

    T &operator[](int index)
    {
        return data[index];
    }

    int size()
    {
        return count;
    }
};

#endif
