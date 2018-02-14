#ifndef CPPTRADING__RANGE__FORWARDRANGE_HPP
#define CPPTRADING__RANGE__FORWARDRANGE_HPP


#include <string.h>

#include "./Range.hpp"


template <typename Container, typename T>
class ForwardRangeData : public RangeData<T> {
public:

    ForwardRangeData(Container& container) :
        _container(container) {}

    virtual const bool init(T*& value) {
        _container_iterator = _container.begin();
        if (_container_iterator != _container.end()) {
            value = &*_container_iterator;
            return true;
        }
        return false;
    }

    virtual const bool next(T*& value) {
        ++_container_iterator;
        if (_container_iterator != _container.end()) {
            value = &*_container_iterator;
            return true;
        }
        return false;
    }

private:

    Container& _container;
    typename Container::iterator _container_iterator;

};


template <typename Container, typename T = typename Container::value_type>
class ForwardRange : public Range<T> {
public:
    ForwardRange(Container& container) :
        Range<T>(new ForwardRangeData<Container, T>(container)) {}
};


template <typename Container>
ForwardRange<Container> ForwardRangeFactory(Container& container) {
    return ForwardRange<Container>(container);
};


#endif // CPPTRADING__RANGE__FORWARDRANGE_HPP
