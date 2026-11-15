#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <iostream>
using namespace std;

// Hash table (separate chaining): finds a patient's position in the patient
// array from the patient ID without searching the whole array.
// key = ID, value = array index. Each bucket is a small linked list.
class HashTable
{
    struct Node
    {
        int key;
        int value;
        Node *next;
    };
    Node **buckets;
    int bucketCount;
    int count;

    void rehash()
    {
        int newCount = bucketCount * 2 + 1;
        Node **bigger = new Node *[newCount];
        for (int i = 0; i < newCount; i++)
        {
            bigger[i] = NULL;
        }
        for (int i = 0; i < bucketCount; i++)
        {
            Node *p = buckets[i];
            while (p != NULL)
            {
                Node *next = p->next;
                int spot = p->key % newCount;
                p->next = bigger[spot];
                bigger[spot] = p;
                p = next;
            }
        }
        delete[] buckets;
        buckets = bigger;
        bucketCount = newCount;
    }

public:
    HashTable()
    {
        bucketCount = 7;
        count = 0;
        buckets = new Node *[bucketCount];
        for (int i = 0; i < bucketCount; i++)
        {
            buckets[i] = NULL;
        }
    }

    ~HashTable()
    {
        for (int i = 0; i < bucketCount; i++)
        {
            Node *p = buckets[i];
            while (p != NULL)
            {
                Node *next = p->next;
                delete p;
                p = next;
            }
        }
        delete[] buckets;
    }

    HashTable(const HashTable &) = delete;
    HashTable &operator=(const HashTable &) = delete;

    int bucketOf(int key)
    {
        return key % bucketCount;
    }

    int size()
    {
        return count;
    }

    // Adds the key, or updates its value if the key is already stored
    void insert(int key, int value)
    {
        int spot = bucketOf(key);
        Node *p = buckets[spot];
        while (p != NULL)
        {
            if (p->key == key)
            {
                p->value = value;
                return;
            }
            p = p->next;
        }
        if (count >= bucketCount * 2)
        {
            rehash();
            spot = bucketOf(key);
        }
        Node *n = new Node;
        n->key = key;
        n->value = value;
        n->next = buckets[spot];
        buckets[spot] = n;
        count++;
    }

    bool find(int key, int &value)
    {
        Node *p = buckets[bucketOf(key)];
        while (p != NULL)
        {
            if (p->key == key)
            {
                value = p->value;
                return true;
            }
            p = p->next;
        }
        return false;
    }

    bool remove(int key)
    {
        int spot = bucketOf(key);
        Node *p = buckets[spot];
        Node *before = NULL;
        while (p != NULL)
        {
            if (p->key == key)
            {
                if (before == NULL)
                {
                    buckets[spot] = p->next;
                }
                else
                {
                    before->next = p->next;
                }
                delete p;
                count--;
                return true;
            }
            before = p;
            p = p->next;
        }
        return false;
    }

    // Shows every bucket and the keys chained inside it
    void display()
    {
        for (int i = 0; i < bucketCount; i++)
        {
            cout << "  Bucket " << i << ": ";
            Node *p = buckets[i];
            if (p == NULL)
            {
                cout << "(empty)";
            }
            while (p != NULL)
            {
                cout << "[ID " << p->key << " -> index " << p->value << "]";
                if (p->next != NULL)
                {
                    cout << " -> ";
                }
                p = p->next;
            }
            cout << endl;
        }
    }
};

#endif
