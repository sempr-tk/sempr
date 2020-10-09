#ifndef SEMPR_CORE_EXCEPTION_HPP_
#define SEMPR_CORE_EXCEPTION_HPP_

#include <exception>
#include <string>

/// namespace only sempr. TODO: refactor the rest of the code? simplify it, use namespaces to group themes together, not code-similarities?

namespace sempr {
/**
    A basic sempr-exception. Why our own? Because std::exception does not take a string to describe
    what happened, and std::runtime_error is more like an error you cannot recover from, at all.
    So this is just a class for exceptions thrown by sempr, with a string description.
    TODO: make it a base of Geometry::TransformException
*/
class Exception : public std::exception {
    std::string message_;
public:
    explicit Exception(const std::string& m) : message_(m) {}
    virtual const char* what() const throw() {
        return message_.c_str();
    }
};

}

#endif /* end of include guard SEMPR_CORE_EXCEPTION_HPP_ */
