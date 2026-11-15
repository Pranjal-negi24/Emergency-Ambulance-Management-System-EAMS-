#ifndef PRIORITYQUEUELINKEDLIST_H
#define PRIORITYQUEUELINKEDLIST_H
#include <iostream>
#include <string>
using namespace std;

// Priority queue built as a max-heap: the patient with the highest severity is
// always at the top. If two patients have the same severity, the one who was
// added first comes out first (sequence number).
class EmergencyQueue
{
    struct Entry
    {
        int id;
        int severity;
        int sequence;
    };
    Entry *heap;
    int capacity;
    int count;
    int nextSequence;

    // true when entry a must come out before entry b
    bool before(Entry &a, Entry &b)
    {
        if (a.severity != b.severity)
        {
            return a.severity > b.severity;
        }
        return a.sequence < b.sequence;
    }

    void swapEntries(int i, int j)
    {
        Entry t = heap[i];
        heap[i] = heap[j];
        heap[j] = t;
    }

    void moveUp(int i)
    {
        while (i > 0)
        {
            int parent = (i - 1) / 2;
            if (before(heap[i], heap[parent]))
            {
                swapEntries(i, parent);
                i = parent;
            }
            else
            {
                break;
            }
        }
    }

    void moveDown(int i)
    {
        while (true)
        {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int best = i;
            if (left < count && before(heap[left], heap[best]))
            {
                best = left;
            }
            if (right < count && before(heap[right], heap[best]))
            {
                best = right;
            }
            if (best == i)
            {
                break;
            }
            swapEntries(i, best);
            i = best;
        }
    }

public:
    EmergencyQueue()
    {
        capacity = 16;
        count = 0;
        nextSequence = 0;
        heap = new Entry[capacity];
    }

    ~EmergencyQueue()
    {
        delete[] heap;
    }

    EmergencyQueue(const EmergencyQueue &) = delete;
    EmergencyQueue &operator=(const EmergencyQueue &) = delete;

    bool empty()
    {
        return count == 0;
    }

    int size()
    {
        return count;
    }

    void add(int id, int level)
    {
        if (count == capacity)
        {
            capacity = capacity * 2;
            Entry *bigger = new Entry[capacity];
            for (int i = 0; i < count; i++)
            {
                bigger[i] = heap[i];
            }
            delete[] heap;
            heap = bigger;
        }
        heap[count].id = id;
        heap[count].severity = level;
        heap[count].sequence = nextSequence++;
        moveUp(count);
        count++;
    }

    // Looks at the highest priority patient without removing them
    bool peekHighest(int &id)
    {
        if (count == 0)
        {
            return false;
        }
        id = heap[0].id;
        return true;
    }

    // Removes and returns the highest priority patient ID (-1 when empty)
    int takeHighestPriority()
    {
        if (count == 0)
        {
            return -1;
        }
        int answer = heap[0].id;
        heap[0] = heap[count - 1];
        count--;
        moveDown(0);
        return answer;
    }

    // Copies all waiting patients into ids[] and levels[], highest priority first.
    // The heap itself is not changed. Returns how many were copied.
    int snapshot(int ids[], int levels[], int maxCount)
    {
        int n = count < maxCount ? count : maxCount;
        Entry *copy = new Entry[count];
        for (int i = 0; i < count; i++)
        {
            copy[i] = heap[i];
        }
        for (int i = 1; i < count; i++)
        {
            Entry key = copy[i];
            int j = i - 1;
            while (j >= 0 && before(key, copy[j]))
            {
                copy[j + 1] = copy[j];
                j--;
            }
            copy[j + 1] = key;
        }
        for (int i = 0; i < n; i++)
        {
            ids[i] = copy[i].id;
            levels[i] = copy[i].severity;
        }
        delete[] copy;
        return n;
    }

    // Shows the heap exactly as it is stored in the array (used by the demo)
    void showHeapArray()
    {
        cout << "  Heap array: ";
        if (count == 0)
        {
            cout << "(empty)";
        }
        for (int i = 0; i < count; i++)
        {
            cout << "[ID " << heap[i].id << ", severity " << heap[i].severity << "] ";
        }
        cout << endl;
    }
};

// Singly linked list of integers. Used for the ambulances that are on a trip:
// an ambulance is inserted when it is dispatched and deleted when it is released.
class SimpleLinkedList
{
    struct Node
    {
        int data;
        Node *next;
    };
    Node *head;
    int count;

public:
    SimpleLinkedList()
    {
        head = NULL;
        count = 0;
    }

    ~SimpleLinkedList()
    {
        while (head != NULL)
        {
            Node *p = head;
            head = head->next;
            delete p;
        }
    }

    SimpleLinkedList(const SimpleLinkedList &) = delete;
    SimpleLinkedList &operator=(const SimpleLinkedList &) = delete;

    void insertEnd(int x)
    {
        Node *n = new Node;
        n->data = x;
        n->next = NULL;
        count++;
        if (head == NULL)
        {
            head = n;
            return;
        }
        Node *p = head;
        while (p->next != NULL)
        {
            p = p->next;
        }
        p->next = n;
    }

    // Deletes the first node that holds x. Returns false if x is not in the list.
    bool remove(int x)
    {
        Node *p = head;
        Node *before = NULL;
        while (p != NULL)
        {
            if (p->data == x)
            {
                if (before == NULL)
                {
                    head = p->next;
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

    bool contains(int x)
    {
        Node *p = head;
        while (p != NULL)
        {
            if (p->data == x)
            {
                return true;
            }
            p = p->next;
        }
        return false;
    }

    int size()
    {
        return count;
    }

    // k = 0 is the first node
    bool getAt(int k, int &x)
    {
        Node *p = head;
        int i = 0;
        while (p != NULL)
        {
            if (i == k)
            {
                x = p->data;
                return true;
            }
            i++;
            p = p->next;
        }
        return false;
    }

    void display()
    {
        if (head == NULL)
        {
            cout << "  (empty list)\n";
            return;
        }
        cout << "  head -> ";
        Node *p = head;
        while (p != NULL)
        {
            cout << p->data << " -> ";
            p = p->next;
        }
        cout << "NULL\n";
    }
};

// Singly linked list of text records (patient, dispatch, admission, release,
// discharge and manual records). Every new record becomes a new node at the end.
class RecordList
{
    struct Node
    {
        int number;
        string text;
        Node *next;
    };
    Node *head;
    Node *tail;
    int count;

    static string lower(string s)
    {
        for (int i = 0; i < (int)s.length(); i++)
        {
            if (s[i] >= 'A' && s[i] <= 'Z')
            {
                s[i] = s[i] - 'A' + 'a';
            }
        }
        return s;
    }

public:
    RecordList()
    {
        head = NULL;
        tail = NULL;
        count = 0;
    }

    ~RecordList()
    {
        while (head != NULL)
        {
            Node *p = head;
            head = head->next;
            delete p;
        }
    }

    RecordList(const RecordList &) = delete;
    RecordList &operator=(const RecordList &) = delete;

    void addRecord(string text)
    {
        Node *n = new Node;
        count++;
        n->number = count;
        n->text = text;
        n->next = NULL;
        if (head == NULL)
        {
            head = n;
            tail = n;
            return;
        }
        tail->next = n;
        tail = n;
    }

    int size()
    {
        return count;
    }

    // k = 0 is the first record
    bool getRecord(int k, string &text)
    {
        Node *p = head;
        int i = 0;
        while (p != NULL)
        {
            if (i == k)
            {
                text = p->text;
                return true;
            }
            i++;
            p = p->next;
        }
        return false;
    }

    void display()
    {
        if (head == NULL)
        {
            cout << "No records yet.\n";
            return;
        }
        Node *p = head;
        while (p != NULL)
        {
            cout << "Record " << p->number << ": " << p->text << endl;
            p = p->next;
        }
    }

    // Walks through the list and prints every record that contains the word
    int search(string word)
    {
        string wanted = lower(word);
        int found = 0;
        Node *p = head;
        while (p != NULL)
        {
            if (lower(p->text).find(wanted) != string::npos)
            {
                cout << "Record " << p->number << ": " << p->text << endl;
                found++;
            }
            p = p->next;
        }
        return found;
    }
};

#endif
