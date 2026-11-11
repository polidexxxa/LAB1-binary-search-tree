#ifndef _OPTION_HPP_
#define _OPTION_HPP_

#include "Exceptions.hpp"

template<typename T>
class Option {
private:
    T value;
    bool hasValue;

public:
    Option() : hasValue(false) {}
    
    explicit Option(const T& val) : value(val), hasValue(true) {}
    
    bool IsNone() const { return !hasValue; }
    
    bool IsSome() const { return hasValue; }
    
    T GetValue() const {
        if (!hasValue) {
            throw EmptyContainerException("Option has no value");
        }
        return value;
    }
    
    T GetValueOr(const T& defaultValue) const {
        return hasValue ? value : defaultValue;
    }
    
    explicit operator bool() const {
        return hasValue;
    }
    
};

#endif // _OPTION_HPP_