#ifndef _LISTSEQUENCE_HPP_
#define _LISTSEQUENCE_HPP_

#include "Sequence.hpp"
#include "LinkedList.hpp"
#include "Exceptions.hpp"
#include <functional>

//Mutable
template<typename T>
class ListSequence : public Sequence<T> {
private:
    LinkedList<T>* list;

protected:
    Sequence<T>* CreateEmptySameType() const override {
        return new ListSequence<T>();
    }
    
    void AppendInternal(const T& item) override { 
        list->Append(item); 
    }

    void PrependInternal(const T& item) override { 
        list->Prepend(item); 
    }

    void InsertAtInternal(const T& item, int index) override { 
        list->InsertAt(item, index); 
    }

    void SetInternal(int index, const T& item) override {
        list->GetRef(index) = item;
    }

public:
    ListSequence(T* items, int count) {
        this->list = new LinkedList<T>(items, count);
    }

    ListSequence() {
        this->list = new LinkedList<T>();
    }

    explicit ListSequence(const LinkedList<T>& linkedList) {
        this->list = new LinkedList<T>(linkedList);
    }

    ListSequence(const ListSequence<T>& other) {
        this->list = new LinkedList<T>(*(other.list));
    }

    ~ListSequence() override {
        delete list;
    }

    Sequence<T>* Instance() override { 
        return this;
    }
    
    ListSequence<T>& operator=(const ListSequence<T>& other) {
        if (this != &other) {
            delete list;
            list = new LinkedList<T>(*(other.list));
        }
        return *this;
    }

    T GetFirst() const override {
        return list->GetFirst();
    }

    T GetLast() const override {
        return list->GetLast();
    }

    T Get(int index) const override {
        return list->Get(index);
    }

    int GetLength() const override {
        return list->GetLength();
    }

    const LinkedList<T>& GetInternalList() const {
        return *list;
    }

    template<typename ResultType>
    Sequence<ResultType>* Map(std::function<ResultType(const T&)> func) const {
        ResultType* temp = new ResultType[this->GetLength()];

        IEnumerator<T>* enumerator = this->GetEnumerator();
        for (int i = 0; enumerator->MoveNext(); i++) {
            temp[i] = func(enumerator->GetCurrent());
        }
        delete enumerator;

        auto* result = new ListSequence<ResultType>(temp, this->GetLength());
        delete[] temp;
        return result;
    }

    Sequence<T>* Clone() const override {
        return new ListSequence<T>(*this);
    }

    T& operator[](int index) override {
        return list->GetRef(index);
    }
    
    const T& operator[](int index) const override {
        return list->GetRef(index);
    }

    IEnumerator<T>* GetEnumerator() const override {
        return new ListEnumerator<T>(list);
    }

}; 

//=============================================================================
//                              IMMUTABLE
//=============================================================================

template<typename T>
class ImmutableListSequence : public ListSequence<T> {
protected:
    Sequence<T>* CreateEmptySameType() const override {
        return new ImmutableListSequence<T>();
    }
    
public:
    using ListSequence<T>::ListSequence;
    
    ImmutableListSequence() : ListSequence<T>() {}

    ImmutableListSequence(const ImmutableListSequence<T>& other) 
        : ListSequence<T>(other) {}

    Sequence<T>* Clone() const override {
        return new ImmutableListSequence<T>(*this);
    }

    Sequence<T>* Instance() override { return this->Clone(); }
    
    T& operator[](int index) override {
        throw UnsupportedOperationException("Immutable sequence does not support mutable iterators");
    }
    
    using ListSequence<T>::operator[];

};

#endif //_LISTSEQUENCE_HPP_
