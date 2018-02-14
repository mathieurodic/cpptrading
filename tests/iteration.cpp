#include <iostream>

#include "range/Range.hpp"

#include <map>


template <typename Key, typename Value>
class MapRangeData : public RangeData<Value> {
public:

    inline MapRangeData(std::map<Key, Value>& data) :
        _data(data),
        _is_full(true) {}
    inline MapRangeData(std::map<Key, Value>& data, const Key& key_begin, const Key& key_end) :
        _data(data),
        _is_full(false),
        _key_begin(key_begin),
        _key_end(key_end) {}

    virtual const bool init(Value& value) {
        if (_is_full) {
            _data_iterator = _data.begin();
        } else {
            _data_iterator = _data.upper_bound(_key_begin);
        }
        if (_data_iterator != _data.end()) {
            value = _data_iterator->second;
            return true;
        }
        return false;
    }

    virtual const bool next(Value& value) {
        ++_data_iterator;
        if (_data_iterator != _data.end() && (_is_full || _data_iterator->first <= _key_end)) {
            value = _data_iterator->second;
            return true;
        }
        return false;
    }

private:
    std::map<Key, Value> _data;
    typename std::map<Key, Value>::iterator _data_iterator;
    Key _key_begin;
    Key _key_end;
    bool _is_full;
};


template <typename Key, typename Value>
class MapRange : public Range<Value> {
public:

    MapRange(std::map<Key, Value>& data)
        : Range<Value>(new MapRangeData<Key, Value>(data)) {}

    MapRange(std::map<Key, Value>& data, const Key& key_begin, const Key& key_end)
        : Range<Value>(new MapRangeData<Key, Value>(data, key_begin, key_end)) {}

};


template <typename Key, typename Value>
inline MapRange<Key, Value> MapRangeFactory(std::map<Key, Value>& data) {
    return MapRange<Key, Value>(data);
}
template <typename Key, typename Value, typename Key1, typename Key2>
inline MapRange<Key, Value> MapRangeFactory(std::map<Key, Value>& data, const Key1& key_begin, const Key2& key_end) {
    return MapRange<Key, Value>(data, key_begin, key_end);
}


#include <iostream>


int main(int argc, char const *argv[]) {
    std::map<std::string, int> map;
    for (int i=0; i<100; ++i) {
        map[std::to_string(i)] = i;
    }
    for (const int& i : MapRangeFactory(map)) {
        std::cout << i << '\n';
    }
    std::cout << "\nShowed full range\n\n";
    for (const int& i : MapRangeFactory(map, "1", "2")) {
        std::cout << i << '\n';
    }
    std::cout << "\nShowed limited range (1-2)\n\n";
    auto range = MapRangeFactory(map, "1", "2");
    auto range_copy = range;
    for (const int& i : range_copy) {
        std::cout << i << '\n';
    }
    std::cout << "\nShowed limited range from copy (1-2)\n\n";
    auto filtered_range = range.filter([] (const int& input) -> bool {
        return input % 2 == 0;
    });
    for (const int& i : filtered_range) {
        std::cout << i << '\n';
    }
    std::cout << "\nShowed filtered range from copy (even)\n\n";
    filtered_range = range.filter([] (const int& input) -> bool {
        return input % 2 == 0;
    });
    auto filtered_range_2 = filtered_range.filter([] (const int& input) -> bool {
        return input % 3 == 0;
    });
    for (const int& i : filtered_range_2) {
        std::cout << i << '\n';
    }
    std::cout << "\nShowed filtered range from copy (even, divisible by 3)\n\n";
    filtered_range = range.filter([] (const int& input) -> bool {
        return input % 2 == 0;
    }).filter([] (const int& input) -> bool {
        return input % 3 == 0;
    });
    for (const int& i : filtered_range_2) {
        std::cout << i << '\n';
    }
    std::cout << "\nShowed chained ranges (even, divisible by 3)\n\n";
    return 0;
}
