#ifndef _LINKEDLIST_HPP_
#define _LINKEDLIST_HPP_

#include "Exceptions.hpp"
#include "IEnumerator.hpp"

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node* prev;
        
        Node(const T& value, Node* nextNode = nullptr, Node* prevNode = nullptr)
            : data(value), next(nextNode), prev(prevNode) {}
    };

    Node* head;
    Node* tail;
    int length;

    void copyFrom(const LinkedList<T>& other) {
        head = tail = nullptr;
        length = 0;
        
        Node* current = other.head;
        while (current != nullptr) {
            Append(current->data);
            current = current->next;
        }
    }

    void clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = tail = nullptr;
        length = 0;
    }


    Node* getNodeAt(int index) const {
        if (index < 0 || index >= length) {
            throw IndexOutOfRangeException("Index " + std::to_string(index) + " out of range [0, " + std::to_string(length) + ")");
        }

        Node* current;
        if (index < length / 2) {
            current = head;
            for (int i = 0; i < index; ++i) {
                current = current->next;
            }
        } 
        else {
            current = tail;
            for (int i = length - 1; i > index; --i) {
                current = current->prev;
            }
        }
        
        return current;
    }


public:
    LinkedList(T* items, int count) : head(nullptr), tail(nullptr), length(0) {
        if (count < 0) {
            throw InvalidArgumentException("Count cannot be negative");
        }
        if (count > 0 && items == nullptr) {
            throw InvalidArgumentException("Items array cannot be null for non-zero count");
        }
        
        for (int i = 0; i < count; ++i) {
            Append(items[i]);
        }
    }

    LinkedList() : head(nullptr), tail(nullptr), length(0) {}

    LinkedList(const LinkedList<T>& other) : head(nullptr), tail(nullptr), length(0) {
        copyFrom(other);
    }

    LinkedList<T>& operator=(const LinkedList<T>& other) {
        if (this != &other) {
            clear();
            copyFrom(other);
        }
        return *this;
    }

    LinkedList(LinkedList<T>&& other) noexcept 
        : head(other.head), tail(other.tail), length(other.length) {
        other.head = nullptr;
        other.tail = nullptr;
        other.length = 0;
    }

    ~LinkedList() {
        clear();
    }

    template<typename U>
    friend class ListEnumerator;


    T GetFirst() const {
        if (length == 0) {
            throw EmptyContainerException("Cannot get first element from empty list");
        }
        return head->data;
    }

    T GetLast() const {
        if (length == 0) {
            throw EmptyContainerException("Cannot get last element from empty list");
        }
        return tail->data;
    }

    T Get(int index) const {
        return getNodeAt(index)->data;
    } 
    
    LinkedList<T>* GetSubList(int startIndex, int endIndex) const {
        if (startIndex < 0 || startIndex >= length || 
            endIndex < 0 || endIndex >= length || 
            startIndex > endIndex) {
            throw IndexOutOfRangeException("Invalid sublist indices");
        }

        LinkedList<T>* result = new LinkedList<T>();
        Node* current = getNodeAt(startIndex);
        
        for (int i = startIndex; i <= endIndex; ++i) {
            result->Append(current->data);
            current = current->next;
        }
        
        return result;
    }


    int GetLength() const {
        return length;
    }

    void Append(T item) {
        Node* newNode = new Node(item, nullptr, tail);

        if (length == 0) {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }

        length++;
    }

    void Prepend(T item) {
        Node* newNode = new Node(item, head, nullptr);

        if (length == 0) {
            head = tail = newNode;
        }
        else {
            head->prev = newNode;
            head = newNode;
        }

        length++;
    }

    void InsertAt(T item, int index) {
        if (index < 0 || index > length) {
            throw IndexOutOfRangeException("Index " + std::to_string(index) + " out of range [0, " + std::to_string(length) + "]");
        }

        if (index == 0) {
            Prepend(item);
        }
        else if (index == length) {
            Append(item);
        }
        else {
            Node* current = getNodeAt(index);
            
            Node* newNode = new Node(item, current, current->prev);
            current->prev->next = newNode;
            current->prev = newNode;
            length++;
        }
    }

    LinkedList<T> Concat(const LinkedList<T>& list) const {
        LinkedList<T> result(*this); 
        
        Node* current = list.head;
        while (current != nullptr) {
            result.Append(current->data);
            current = current->next;
        }
        
        return result;
    }

    T& GetRef(int index) {
        Node* current = getNodeAt(index);

        return current->data;
    }

    const T& GetRef(int index) const {
        Node* current = getNodeAt(index);
        
        return current->data;
    }

    Node* GetHead() { return head; }
    const Node* GetHead() const { return head; }
    
    Node* GetTail() { return tail; }
    const Node* GetTail() const { return tail; }

};


//==============================================================

//==============================================================


template<typename T>
class ListEnumerator : public IEnumerator<T> {
private:
    LinkedList<T>* list;
    typename LinkedList<T>::Node* currentNode;
    bool isConst;

public:
    ListEnumerator(LinkedList<T>* list) : list(list), currentNode(nullptr), isConst(false) {
        if (list == nullptr) {
            throw InvalidArgumentException("ListEnumerator: list cannot be null");
        }
    }
    
    ListEnumerator(const LinkedList<T>* list) : list(const_cast<LinkedList<T>*>(list)), currentNode(nullptr), isConst(true) {
        if (list == nullptr) {
            throw InvalidArgumentException("ListEnumerator: list cannot be null");
        }
    }
    
    ListEnumerator(const ListEnumerator& other) : list(other.list), currentNode(other.currentNode), isConst(other.isConst) {}
    
    IEnumerator<T>* Clone() const override {
        return new ListEnumerator(*this);
    }
    
    bool MoveNext() override {
        if (list == nullptr) return false; 

        if (currentNode == nullptr) {
            currentNode = list->GetHead();
        } 
        else {
            currentNode = currentNode->next;
        }
        return currentNode != nullptr;
    }
    
    const T& GetCurrent() const override {
        if (list == nullptr) {
            throw InvalidArgumentException("Enumerator is not attached to a list");
        }
        if (currentNode == nullptr) {
            throw IndexOutOfRangeException("Enumerator out of range");
        }
        return currentNode->data;
    }
    
    T& GetCurrent() override {
        if (list == nullptr) {
            throw InvalidArgumentException("Enumerator is not attached to a list");
        }
        if (isConst) {
            throw UnsupportedOperationException("Cannot modify through const enumerator");
        }
        if (currentNode == nullptr) {
            throw IndexOutOfRangeException("Enumerator out of range");
        }
        return currentNode->data;
    }
    
    void Reset() override {
        currentNode = nullptr;
    }
};



#endif //_LINKEDLIST_HPP_