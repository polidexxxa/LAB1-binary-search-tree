#ifndef _STACK_HPP_
#define _STACK_HPP_

#include "ListBasedContainer.hpp"

template <typename T>
class Stack : public ListBasedContainer<T, Stack> {
private:
    using Base = ListBasedContainer<T, Stack>;

public:
    Stack() {}
    Stack(const Stack<T>& other) : Base(other) {}
    explicit Stack(const Sequence<T>& seq) : Base(seq) {}

    Stack<T>& operator=(const Stack<T>& other) {
        Base::operator=(other);
        return *this;
    }

    void Push(T item) {
        this->storage.Append(item);
    }

    T Pop() {
        if (this->IsEmpty()) {
            throw EmptyContainerException("Cannot pop from empty stack");
        }

        T top = this->storage.GetLast();
        this->RemoveBackInternal();
        return top;
    }

    T Peek() const {
        if (this->IsEmpty()) {
            throw EmptyContainerException("Cannot peek into empty stack");
        }

        return this->storage.GetLast();
    }

    using Base::IsEmpty;
    using Base::GetSize;
    using Base::Clear;
    using Base::Concat;
    using Base::GetSubcontainer;
    using Base::Map;
    using Base::Where;
    using Base::Reduce;
    using Base::ContainsSubsequence;
};

#endif //_STACK_HPP_