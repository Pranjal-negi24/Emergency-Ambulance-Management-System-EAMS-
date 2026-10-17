#ifndef PRIORITYQUEUELINKEDLIST_H
#define PRIORITYQUEUELINKEDLIST_H
#include <iostream>
#include <string>
using namespace std;

// Priority queue: the patient with the highest severity is taken out first
class EmergencyQueue
{
    int patientIds[100];
    int severity[100];
    int count;

public:
    EmergencyQueue()
    {
        count = 0;
    }

    bool empty()
    {
        return count == 0;
    }

    bool add(int id, int level)
    {
        if (count == 100)
        {
            return false;
        }
        patientIds[count] = id;
        severity[count] = level;
        count++;
        return true;
    }

    int takeHighestPriority()
    {
        if (count == 0)
        {
            return -1;
        }
        int best = 0;
        for (int i = 1; i < count; i++)
        {
            if (severity[i] > severity[best])
            {
                best = i;
            }
        }
        int answer = patientIds[best];
        for (int i = best; i < count - 1; i++)
        {
            patientIds[i] = patientIds[i + 1];
            severity[i] = severity[i + 1];
        }
        count--;
        return answer;
    }

    void display()
    {
        if (count == 0)
        {
            cout << "No pending emergencies.\n";
            return;
        }
        cout << "Pending emergencies (higher severity first when dispatched):\n";
        for (int i = 0; i < count; i++)
        {
            cout << "Patient ID: " << patientIds[i] << " | Severity: " << severity[i] << endl;
        }
    }
};

// Singly linked list of integers
class SimpleLinkedList
{
    struct Node
    {
        int data;
        Node *next;
    };
    Node *head;

public:
    SimpleLinkedList()
    {
        head = NULL;
    }

    void insertEnd(int x)
    {
        Node *n = new Node;
        n->data = x;
        n->next = NULL;
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

    void display()
    {
        Node *p = head;
        while (p != NULL)
        {
            cout << p->data << " ";
            p = p->next;
        }
        cout << endl;
    }
};

// Singly linked list of text records (dispatch and admission records).
// Every new record becomes a new node at the end of the list.
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
};

#endif
