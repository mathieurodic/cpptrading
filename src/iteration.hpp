#ifndef CPPTRADING__ITERATION_HPP
#define CPPTRADING__ITERATION_HPP


#include <stdlib.h>


template <typename T>
class RangeData {
public:

    virtual const bool init(T& value) = 0;
    virtual const bool next(T& value) = 0;

};


template <typename T>
class Iterator {
public:

    inline Iterator() :
        _is_finished(true) {}
    inline Iterator(RangeData<T>* range_data) :
        _range_data(range_data),
        _is_finished(! range_data->init(_value)) {}

    inline void operator ++ () {
        if (!_is_finished && _range_data) {
            _is_finished = ! _range_data->next(_value);
        }
    }

    inline const T& operator * () const {
        return _value;
    }

    template <typename OtherItem>
    inline const bool operator != (const OtherItem& other) const {
        return _is_finished != other._is_finished;
    }

private:

    bool _is_finished;
    RangeData<T>* _range_data;
    T _value;

};


template <typename T>
class Range {
public:

    inline Range() :
        _range_data(NULL),
        _counter(0),
        _root_counter(_counter) {}

    inline Range(const Range<T>& source) :
        _range_data(source._range_data),
        _counter(0),
        _root_counter(source._root_counter)
    {
        ++source._root_counter;
    }
    inline Range(RangeData<T>* _range_data) :
        _range_data(_range_data),
        _counter(1),
        _root_counter(_counter) {}

    inline ~Range() {
        if (--_root_counter < 1 && _range_data != NULL) {
            delete _range_data;
            _range_data = NULL;
        }
    }

    inline Iterator<T> begin() {
        if (_range_data == NULL) {
            return _end_iterator;
        }
        return Iterator<T>(_range_data);
    }
    inline Iterator<T> end() {
        return _end_iterator;
    }

private:

    int _counter;
    int& _root_counter;
    RangeData<T>* _range_data;
    static const Iterator<T> _end_iterator;

};

template <typename T>
const Iterator<T> Range<T>::_end_iterator;


#endif // CPPTRADING__ITERATION_HPP
