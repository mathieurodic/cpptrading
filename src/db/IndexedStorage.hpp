#ifndef CTRADING__DB__INDEXEDSTORAGE__HPP
#define CTRADING__DB__INDEXEDSTORAGE__HPP


#include <string>
#include <map>
#include <experimental/tuple>

#include "IO/directories.hpp"



template <typename record_t, typename logger_t>
class IndexedStorage {
public:

    inline IndexedStorage(const std::string& path) :
        _path(path),
        _path_is_initialized(init_directory(_path)),
        _logger_path(path + "/log"),
        _logger_writer(_logger_path)
    {}

    inline bool init_directory(const std::string& path) {
        make_directory(path);
        return true;
    }

    inline void insert(record_t& record) {
        _logger_writer.append(record);
    }

    template <typename index_t>
    inline const bool integrity_check(index_t& index) {
        record_t record;
        typename logger_t::Reader reader(_logger_path);
        while (reader.next(record) == sizeof(record)) {
            if (index.count(record) < 1) {
                return false;
            }
        }
        return true;
    }

    inline typename logger_t::template Range<record_t> get_all() {
        return _logger_writer.template get_all<record_t>();
    }

protected:
    const std::string _path;
    const bool _path_is_initialized;
    const std::string _logger_path;
    typename logger_t::Writer _logger_writer;
};



#ifndef typeof
#define typeof(THING) __typeof(THING)
#endif // typeof

#ifndef typeofproperty
#define typeofproperty(CLASS, PROPERTY) typeof( ((CLASS*) NULL)->PROPERTY )
#endif // typeofproperty

#ifndef offsetofproperty
#define offsetofproperty(CLASS, PROPERTY) ( (char*) &(((CLASS*) NULL)->PROPERTY) - (char*) NULL )
#endif // offsetofproperty


#define INDEXED_STORAGE_GETTER(PROPERTY, KEY_TYPE, RECORD_TYPE, METHOD) \
    inline const RECORD_TYPE METHOD##_by_##PROPERTY(KEY_TYPE& key) { return _index_##PROPERTY.METHOD(key); }

#define INDEXED_STORAGE_GETTERS(PROPERTY, KEY_TYPE, RECORD_TYPE) \
    INDEXED_STORAGE_GETTER(PROPERTY, KEY_TYPE, RECORD_TYPE, get) \
    INDEXED_STORAGE_GETTER(PROPERTY, KEY_TYPE, RECORD_TYPE, get_le) \
    INDEXED_STORAGE_GETTER(PROPERTY, KEY_TYPE, RECORD_TYPE, get_lt) \
    INDEXED_STORAGE_GETTER(PROPERTY, KEY_TYPE, RECORD_TYPE, get_ge) \
    INDEXED_STORAGE_GETTER(PROPERTY, KEY_TYPE, RECORD_TYPE, get_gt) \
    inline const bool contains_##PROPERTY(RECORD_TYPE& searched_record) { return _index_##PROPERTY.contains(searched_record); } \
    inline const size_t count_##PROPERTY(RECORD_TYPE& searched_record) { return _index_##PROPERTY.count(searched_record); } \
    inline UpscaleBTreeRange<KEY_TYPE, RECORD_TYPE> get_##PROPERTY##_all() { return _index_##PROPERTY.get_all(); } \
    inline UpscaleBTreeRange<KEY_TYPE, RECORD_TYPE> get_##PROPERTY##_range(KEY_TYPE key_begin, KEY_TYPE key_end) { return _index_##PROPERTY.get_range(key_begin, key_end); } \

#define INDEXED_STORAGE(CLASS, PROPERTY, LOGGER, INDEX) \
    class IndexedStorage##__##CLASS##_##PROPERTY : public IndexedStorage<CLASS, LOGGER> { \
    public: \
        inline IndexedStorage##__##CLASS##_##PROPERTY(const std::string& path) : \
            IndexedStorage<CLASS, LOGGER>(path), \
            _index_##PROPERTY(path + "/index." + #PROPERTY) {} \
        inline void insert(CLASS& record) { \
            _logger_writer.append(record); \
            _index_##PROPERTY.insert(record); \
        } \
        INDEXED_STORAGE_GETTERS(PROPERTY, typeofproperty(CLASS, PROPERTY), CLASS) \
        const bool integrity_check() { \
            return IndexedStorage<CLASS, LOGGER>::integrity_check(_index_##PROPERTY); \
        } \
    protected: \
        INDEX<typeofproperty(CLASS, PROPERTY), offsetofproperty(CLASS, PROPERTY), CLASS> _index_##PROPERTY; \
    }

#define INDEXED_STORAGE_2(CLASS, PROPERTY1, PROPERTY2, LOGGER, INDEX) \
    class IndexedStorage##__##CLASS##_##PROPERTY1##__##PROPERTY2 : public IndexedStorage<CLASS, LOGGER> { \
    public: \
        inline IndexedStorage##__##CLASS##_##PROPERTY1##__##PROPERTY2(const std::string& path) : \
            IndexedStorage<CLASS, LOGGER>(path), \
            _index_##PROPERTY1(path + "/index." + #PROPERTY1), \
            _index_##PROPERTY2(path + "/index." + #PROPERTY2) {} \
        inline void insert(CLASS& record) { \
            _logger_writer.append(record); \
            _index_##PROPERTY1.insert(record); \
            _index_##PROPERTY2.insert(record); \
        } \
        INDEXED_STORAGE_GETTERS(PROPERTY1, typeofproperty(CLASS, PROPERTY1), CLASS) \
        INDEXED_STORAGE_GETTERS(PROPERTY2, typeofproperty(CLASS, PROPERTY2), CLASS) \
        const bool integrity_check() { \
            return IndexedStorage<CLASS, LOGGER>::integrity_check(_index_##PROPERTY1) && IndexedStorage<CLASS, LOGGER>::integrity_check(_index_##PROPERTY2); \
        } \
    protected: \
        INDEX<typeofproperty(CLASS, PROPERTY1), offsetofproperty(CLASS, PROPERTY1), CLASS> _index_##PROPERTY1; \
        INDEX<typeofproperty(CLASS, PROPERTY2), offsetofproperty(CLASS, PROPERTY2), CLASS> _index_##PROPERTY2; \
    }


#endif // CTRADING__DB__INDEXEDSTORAGE__HPP
