#ifndef _SEQUENCE_HPP_
#define _SEQUENCE_HPP_

#include "Exceptions.hpp"
#include "Option.hpp"
#include "IEnumerator.hpp"
#include <functional>

template<typename T>
class Sequence {
protected:
    virtual Sequence<T>* CreateEmptySameType() const = 0;

    virtual void AppendInternal(const T& item) = 0;
    virtual void PrependInternal(const T& item) = 0;
    virtual void InsertAtInternal(const T& item, int index) = 0;
    virtual void SetInternal(int index, const T& item) = 0;

public:
    virtual ~Sequence() = default;
     
    virtual Sequence<T>* Instance() = 0;
    
    Sequence<T>* Append(const T& item) {
        Sequence<T>* target = Instance();
        target->AppendInternal(item);
        return target;
    }

    Sequence<T>* Prepend(const T& item) {
        Sequence<T>* target = Instance();
        target->PrependInternal(item);
        return target;
    }

    Sequence<T>* InsertAt(const T& item, int index) {
        Sequence<T>* target = Instance();
        target->InsertAtInternal(item, index);
        return target;
    }

    Sequence<T>* Set(int index, const T& item) {
        Sequence<T>* target = Instance();
        target->SetInternal(index, item);
        return target;
    }
    
    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    virtual T Get(int index) const = 0;
    virtual int GetLength() const = 0;
    
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const {
        int length = this->GetLength();
        if (startIndex < 0 || startIndex >= length || endIndex < 0 || endIndex >= length || startIndex > endIndex) {
            throw IndexOutOfRangeException("Invalid subsequence indices");
        }

        Sequence<T>* result = this->CreateEmptySameType();
        IEnumerator<T>* enumerator = this->GetEnumerator();
        int index = 0;

        while (index < startIndex && enumerator->MoveNext()) {
            index++;
        }
        while (index <= endIndex && enumerator->MoveNext()) {
            result->AppendInternal(enumerator->GetCurrent());
            index++;
        }
        
        delete enumerator;
        return result;
    }

    Sequence<T>* Concat(const Sequence<T>* other) const {
        if (other == nullptr) {
            throw InvalidArgumentException("Cannot concatenate with null sequence");
        }

        Sequence<T>* result = this->Clone();
        IEnumerator<T>* enumerator = other->GetEnumerator();
        while (enumerator->MoveNext()) {
            result->AppendInternal(enumerator->GetCurrent());
        }
        delete enumerator;

        return result;
    }

    Sequence<T>* Where(std::function<bool(const T&)> predicate) const {
        Sequence<T>* result = this->CreateEmptySameType();
        IEnumerator<T>* enumerator = this->GetEnumerator();
        while (enumerator->MoveNext()) {
            T val = enumerator->GetCurrent();
            if (predicate(val)) {
                result->AppendInternal(val);
            }
        }
        delete enumerator;
        return result;
    }

    template<typename Accumulator>
    Accumulator Reduce(const Accumulator& initial, std::function<Accumulator(Accumulator, const T&)> func) const {
        Accumulator result = initial;
        IEnumerator<T>* enumerator = this->GetEnumerator();
        while (enumerator->MoveNext()) {
            result = func(result, enumerator->GetCurrent());
        }
        delete enumerator;
        return result;
    }

    Option<T> TryGetFirst() const {
        if (GetLength() == 0) return Option<T>();
        return Option<T>(GetFirst());
    }
    
    Option<T> TryGetLast() const {
        if (GetLength() == 0) return Option<T>();
        return Option<T>(GetLast());
    }
    
    Option<T> TryFind(std::function<bool(const T&)> predicate) const {
        IEnumerator<T>* enumerator = this->GetEnumerator();
        while (enumerator->MoveNext()) {
            T val = enumerator->GetCurrent();
            if (predicate(val)) {
                delete enumerator;
                return Option<T>(val);
            }
        }
        delete enumerator;
        return Option<T>();
    }

    Sequence<T>* Slice(int start, int count, Sequence<T>* replacement = nullptr) const {
        int length = this->GetLength();
        
        if (start < 0) start = length + start;
        if (start < 0 || start > length) {
            throw IndexOutOfRangeException("Start index out of range");
        }
        if (count < 0) {
            throw InvalidArgumentException("Count cannot be negative");
        }
        if (start + count > length) count = length - start;

        Sequence<T>* result = this->CreateEmptySameType();
        IEnumerator<T>* enumerator = this->GetEnumerator();
        
        int index = 0;

        while (index < start && enumerator->MoveNext()) {
            result->AppendInternal(enumerator->GetCurrent());
            index++;
        }
        
        for (int i = 0; i < count; ++i) {
            if (enumerator->MoveNext()) {
                index++;
            }
        }

        if (replacement != nullptr) {
            IEnumerator<T>* replEnum = replacement->GetEnumerator();
            while (replEnum->MoveNext()) {
                result->AppendInternal(replEnum->GetCurrent());
            }
            delete replEnum;
        }

        while (enumerator->MoveNext()) {
            result->AppendInternal(enumerator->GetCurrent());
        }
        
        delete enumerator;
        return result;
    }

    virtual T& operator[](int index) = 0;
    virtual const T& operator[](int index) const = 0;

    virtual Sequence<T>* Clone() const = 0;

    virtual IEnumerator<T>* GetEnumerator() const = 0;
};

#endif //_SEQUENCE_HPP_
