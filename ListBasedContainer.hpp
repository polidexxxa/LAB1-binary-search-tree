#ifndef _LISTBASEDCONTAINER_HPP_
#define _LISTBASEDCONTAINER_HPP_

#include "ListSequence.hpp"
#include "Exceptions.hpp"
#include <functional>

template <typename T, template<typename> class Derived>
class ListBasedContainer {
protected:
    ListSequence<T> storage;

    void RemoveFrontInternal() {
        if (storage.GetLength() == 1) {
            storage = ListSequence<T>();
        } 
        else {
            Sequence<T>* subSeq = storage.GetSubsequence(1, storage.GetLength() - 1);
            auto* listSub = dynamic_cast<ListSequence<T>*>(subSeq);
            if (listSub == nullptr) {
                delete subSeq;
                throw InvalidArgumentException("Subsequence is not ListSequence");
            }
            storage = *listSub;
            delete listSub;
        }
    }

    void RemoveBackInternal() {
        if (storage.GetLength() == 1) {
            storage = ListSequence<T>();
        } 
        else {
            Sequence<T>* subSeq = storage.GetSubsequence(0, storage.GetLength() - 2);
            auto* listSub = dynamic_cast<ListSequence<T>*>(subSeq);
            if (listSub == nullptr) {
                delete subSeq;
                throw InvalidArgumentException("Subsequence is not ListSequence");
            }
            storage = *listSub;
            delete listSub;
        }
    }

    ListBasedContainer(ListBasedContainer&& other) noexcept
        : storage(std::move(other.storage)) {}

    explicit ListBasedContainer(Sequence<T>&& seq) {
        IEnumerator<T>* en = seq.GetEnumerator();
        while (en->MoveNext()) {
            storage.Append(std::move(const_cast<T&>(en->GetCurrent())));
        }
        delete en;
    }
    
    explicit ListBasedContainer(ListSequence<T>&& seq) noexcept
        : storage(std::move(seq)) {}

public:
    ListBasedContainer() {}

    ListBasedContainer(const ListBasedContainer<T, Derived>& other)
        : storage(other.storage) {}

    explicit ListBasedContainer(const Sequence<T>& seq) {
        IEnumerator<T>* en = seq.GetEnumerator();
        while (en->MoveNext()) {
            storage.Append(en->GetCurrent());
        }
        delete en;
    }

    ListBasedContainer<T, Derived>& operator=(const ListBasedContainer<T, Derived>& other) {
        if (this != &other) {
            storage = other.storage;
        }
        return *this;
    }

    virtual ~ListBasedContainer() = default;

    bool IsEmpty() const {
        return storage.GetLength() == 0;
    }

    int GetSize() const {
        return storage.GetLength();
    }

    void Clear() {
        storage = ListSequence<T>();
    }

    Derived<T>* Concat(const Derived<T>& other) const {
        Sequence<T>* combined = storage.Concat(&(other.GetStorage()));
        Derived<T>* result = new Derived<T>(std::move(*combined));
        delete combined;
        return result;
    }

    Derived<T>* GetSubcontainer(int start, int end) const {
        if (start < 0 || end >= storage.GetLength() || start > end) {
            throw IndexOutOfRangeException("Invalid subcontainer indices");
        }

        Sequence<T>* subSeq = storage.GetSubsequence(start, end);
        Derived<T>* result = new Derived<T>(std::move(*subSeq));
        delete subSeq;
        return result;
    }

    template <typename ResultType>
    Derived<ResultType>* Map(std::function<ResultType(T)> func) const {
        Sequence<ResultType>* mapped = storage.template Map<ResultType>(func);
        Derived<ResultType>* result = new Derived<ResultType>(std::move(*mapped));
        delete mapped;
        return result;
    }

    Derived<T>* Where(std::function<bool(const T&)> predicate) const {
        Sequence<T>* filtered = storage.Where(predicate);
        Derived<T>* result = new Derived<T>(std::move(*filtered));
        delete filtered;
        return result;
    }

    template <typename Accumulator>
    Accumulator Reduce(const Accumulator& initial, std::function<Accumulator(Accumulator, T)> func) const {
        return storage.template Reduce<Accumulator>(initial, func);
    }

    bool ContainsSubsequence(const Sequence<T>& subsequence) const {
        int subSize = subsequence.GetLength();
        int mainSize = this->GetSize();

        if (subSize == 0) return true;
        if (subSize > mainSize) return false;

        T firstElement = subsequence.GetFirst();
        IEnumerator<T>* mainEnum = storage.GetEnumerator();
        int position = 0;

        while (mainEnum->MoveNext()) {
            if (position + subSize > mainSize) {
                break;
            }

            if (mainEnum->GetCurrent() == firstElement) {
                IEnumerator<T>* checkMain = mainEnum->Clone();
                IEnumerator<T>* subEnum = subsequence.GetEnumerator();

                bool match = true;

                if (!subEnum->MoveNext()) {
                    match = true;
                } 
                else {
                    do {
                        if (subEnum->GetCurrent() != checkMain->GetCurrent()) {
                            match = false;
                            break;
                        }
                        if (!subEnum->MoveNext()) {
                            break;
                        }
                        if (!checkMain->MoveNext()) {
                            match = false;
                            break;
                        }
                    } while (true);
                }

                delete checkMain;
                delete subEnum;

                if (match) {
                    delete mainEnum;
                    return true;
                }
            }

            position++;
        }

        delete mainEnum;
        return false;
    }

    bool ContainsSubsequence(const Derived<T>& subsequence) const {
        return ContainsSubsequence(static_cast<const Sequence<T>&>(subsequence.GetStorage()));
    }

    const ListSequence<T>& GetStorage() const {
        return storage;
    }
};

#endif //_LISTBASEDCONTAINER_HPP_