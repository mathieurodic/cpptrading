#ifndef CPPTRADING__RANGE__ASSOCIATIVERANGE_HPP
#define CPPTRADING__RANGE__ASSOCIATIVERANGE_HPP


#include "./Range.hpp"


template <typename Container, typename Key, typename T>
class AssociativeRangeData : public RangeData<T> {
public:

    AssociativeRangeData(Container& container) :
        _container(container),
        _is_bounded(false) {}

    AssociativeRangeData(Container& container, const Key& begin, const Key& end) :
        _container(container),
        _is_bounded(true),
        _begin(begin),
        _end(end) {}

    virtual const bool init(T& value) {
        if (_is_bounded) {
            _container_iterator = _container.upper_bound(_begin);
        } else {
            _container_iterator = _container.begin();
        }
        if (_container_iterator != _container.end()) {
            value = _container_iterator->second;
            return true;
        }
        return false;
    }

    virtual const bool next(T& value) {
        ++_container_iterator;
        if (_container_iterator != _container.end() && (!_is_bounded || _container_iterator->first <= _end)) {
            value = _container_iterator->second;
            return true;
        }
        return false;
    }

private:

    Container& _container;
    typename Container::iterator _container_iterator;
    const bool _is_bounded;
    typename Container::key_type _begin;
    typename Container::key_type _end;

};


template <typename Container, typename Key = typename Container::key_type, typename T = typename Container::mapped_type>
class AssociativeRange : public Range<T> {
public:
    AssociativeRange(Container& container) :
        Range<T>(new AssociativeRangeData<Container, Key, T>(container)) {}
    AssociativeRange(Container& container, const Key& begin, const Key& end) :
        Range<T>(new AssociativeRangeData<Container, Key, T>(container, begin, end)) {}
};


template <typename Container>
AssociativeRange<Container> AssociativeRangeFactory(Container& container) {
    return AssociativeRange<Container>(container);
};
template <typename Container, typename BeginKey, typename EndKey>
AssociativeRange<Container> AssociativeRangeFactory(Container& container, const BeginKey& begin, const EndKey& end) {
    return AssociativeRange<Container>(container, begin, end);
};


#endif // CPPTRADING__RANGE__ASSOCIATIVERANGE_HPP
