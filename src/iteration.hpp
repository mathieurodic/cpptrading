#ifndef CPPTRADING__ITERATION_HPP
#define CPPTRADING__ITERATION_HPP


template <typename T>
class Iterator {
public:

    Iterator(const bool is_finished=true) :
        _is_finished(is_finished) {}

    virtual const T& operator*() const {
        return _value;
    }

    virtual void operator++() {}

    virtual const bool operator!=(const Iterator<T>& other) const {
        return _is_finished != other._is_finished;
    }

protected:

    bool _is_finished;
    T _value;

};


template <typename T, typename IteratorT = Iterator<T>>
class Range {
public:

    Range() {}

    virtual IteratorT begin() {
        return _end;
    }
    inline const IteratorT& end() const {
        return _end;
    }

private:
    static const IteratorT _end;
};

template <typename T, typename IteratorT>
const IteratorT Range<T, IteratorT>::_end;


#endif // CPPTRADING__ITERATION_HPP
