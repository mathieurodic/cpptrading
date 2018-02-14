#ifndef CPPTRADING__ITERATION_HPP
#define CPPTRADING__ITERATION_HPP


#include <stdlib.h>
#include <memory>


template <typename T>
class RangeData {
public:

    virtual ~RangeData() {}

    virtual const bool init(T& value) = 0;
    virtual const bool next(T& value) = 0;

};


template <typename T>
class Iterator {
public:

    inline Iterator() :
        _is_finished(true) {}
    inline Iterator(std::shared_ptr<RangeData<T>> range_data) :
        _range_data(range_data),
        _is_finished((range_data == NULL) ? true : (!range_data->init(_value))) {}

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
    template <typename OtherItem>
    inline const bool operator == (const OtherItem& other) const {
        return _is_finished == other._is_finished;
    }

private:

    bool _is_finished;
    std::shared_ptr<RangeData<T>> _range_data;
    T _value;

};


template <typename T>
class FilterRange;


template <typename T>
class Range {
public:

    friend class FilterRange<T>;

    inline Range() :
        _range_data(NULL) {}

    inline Range(const Range<T>& source) :
        _range_data(source._range_data) {}
    inline Range(RangeData<T>* _range_data) :
        _range_data(_range_data) {}

    inline Range<T> operator = (const Range<T>& source) {
        _range_data = source._range_data;
        return *this;
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

    FilterRange<T> filter(std::function<bool(const T&)> filter);

    std::shared_ptr<RangeData<T>> _range_data;
    static const Iterator<T> _end_iterator;

};

template <typename T>
const Iterator<T> Range<T>::_end_iterator;


template <typename T>
class FilterRangeData : public RangeData<T> {
public:

    inline FilterRangeData(Range<T>& range, std::function<bool(const T&)> filter) :
        _range_data(range._range_data),
        _filter(filter) {}

    virtual const bool init(T& value) {
        bool result = _range_data->init(value);
        while (result) {
            if (_filter(value)) {
                return true;
            }
            result = _range_data->next(value);
        }
        return false;
    }
    virtual const bool next(T& value) {
        bool result = _range_data->next(value);
        while (result) {
            if (_filter(value)) {
                return true;
            }
            result = _range_data->next(value);
        }
        return false;
    }

private:
    std::shared_ptr<RangeData<T>> _range_data;
    Iterator<T> _iterator;
    std::function<bool(const T&)> _filter;
};

template <typename T>
class FilterRange : public Range<T> {
public:

    FilterRange(const FilterRange<T>& source) :
        Range<T>(source) {}

    FilterRange(Range<T>& range, std::function<bool(const T&)> filter) :
        Range<T>(new FilterRangeData<T>(range, filter)) {}

};


template <typename T>
FilterRange<T> Range<T>::filter(std::function<bool(const T&)> filter) {
    return FilterRange<T>(*this, filter);
}


#endif // CPPTRADING__ITERATION_HPP
