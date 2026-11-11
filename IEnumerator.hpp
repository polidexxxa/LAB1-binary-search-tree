#ifndef _IENUMERATOR_HPP_
#define _IENUMERATOR_HPP_

template<typename T>
class IEnumerator {
public:
    virtual ~IEnumerator() = default;

    virtual bool MoveNext() = 0;
    virtual const T& GetCurrent() const = 0;
    virtual T& GetCurrent() = 0;

    virtual void Reset() = 0;

    virtual IEnumerator<T>* Clone() const = 0;
};

template<typename T>
class IEnumerable {
public:
    virtual ~IEnumerable() = default;

    virtual IEnumerator<T>* GetEnumerator() const = 0;
};

template <typename T>
class ICollection : public IEnumerable<T> {
public:
    virtual ~ICollection() = default;

    virtual size_t GetCount() const = 0;
    virtual bool IsEmpty() const = 0;
    
    virtual void Clear() = 0;
    virtual bool Contains(const T& item) const = 0;
};

#endif //_IENUMERATOR_HPP_