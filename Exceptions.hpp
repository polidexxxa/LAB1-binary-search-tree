#ifndef _EXCEPTIONS_HPP_
#define _EXCEPTIONS_HPP_

#include <stdexcept>
#include <string>

class IndexOutOfRangeException : public std::out_of_range {
public:
    explicit IndexOutOfRangeException(const std::string& message) 
        : std::out_of_range("IndexOutOfRange: " + message) {}
};

class EmptyContainerException : public std::logic_error {
public:
    explicit EmptyContainerException(const std::string& message) 
        : std::logic_error("EmptyContainer: " + message) {}
};

class InvalidArgumentException : public std::invalid_argument {
public:
    explicit InvalidArgumentException(const std::string& message) 
        : std::invalid_argument("InvalidArgument: " + message) {}
};

class DifferentLengthException : public std::logic_error {
public:
    explicit DifferentLengthException(const std::string& message) 
        : std::logic_error("DifferentLength: " + message) {}
};

class UnsupportedOperationException : public std::runtime_error {
public:
    explicit UnsupportedOperationException(const std::string& message) 
        : std::runtime_error("UnsupportedOperation: " + message) {}
};

#endif // _EXCEPTIONS_HPP_