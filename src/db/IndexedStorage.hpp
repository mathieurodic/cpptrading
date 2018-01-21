#ifndef CTRADING__DB__INDEXEDSTORAGE__HPP
#define CTRADING__DB__INDEXEDSTORAGE__HPP


#include <string>

#include "./GzipLog.hpp"
#include "./UpscaleBTree.hpp"


template <typename record_t>
class IndexedStorage {
public:

    inline IndexedStorage(const std::string& path) :
        _path(path),
        _logger_path(path + ".log.gz"),
        _logger(_logger_path)
    {

    }

    template <typename key_t, size_t key_offset>
    inline bool integrity_check(UpscaleBTree<key_t, key_offset, record_t>& index) {
        record_t record;
        while (index.next(record) == sizeof(record)) {
            if (!index.contains(record)) {
                return false;
            }
        }
        return true;
    }

    template <typename key_t, size_t key_offset>
    inline bool integrity_ensure(UpscaleBTree<key_t, key_offset, record_t>& index) {
    }

protected:
    const std::string _path;
    const std::string _logger_path;
    // const std::string _index_path;
    GzipLogWriter _logger;
};


#ifndef typeof
#define typeof(THING) __typeof(THING)
#endif // typeof

#ifndef typeofproperty
#define typeofproperty(CLASS, PROPERTY) typeof( ((CLASS*) NULL)->PROPERTY )
#endif // typeofproperty


#define INDEXED_STORAGE_GETTER(PROPERTY, KEY_TYPE, RECORD_TYPE, METHOD) \
    inline const RECORD_TYPE METHOD##_by_##PROPERTY(KEY_TYPE& key) { return _index_##PROPERTY.METHOD(key); }

#define INDEXED_STORAGE_GETTERS(PROPERTY, KEY_TYPE, RECORD_TYPE) \
    INDEXED_STORAGE_GETTER(PROPERTY, KEY_TYPE, RECORD_TYPE, get) \
    INDEXED_STORAGE_GETTER(PROPERTY, KEY_TYPE, RECORD_TYPE, get_le) \
    INDEXED_STORAGE_GETTER(PROPERTY, KEY_TYPE, RECORD_TYPE, get_lt) \
    INDEXED_STORAGE_GETTER(PROPERTY, KEY_TYPE, RECORD_TYPE, get_ge) \
    INDEXED_STORAGE_GETTER(PROPERTY, KEY_TYPE, RECORD_TYPE, get_gt) \
    inline const size_t contains_##PROPERTY(RECORD_TYPE& searched_record) { return _index_##PROPERTY.contains(searched_record); } \
    inline UpscaleBTreeRange<KEY_TYPE, RECORD_TYPE> get_##PROPERTY##_all() { return _index_##PROPERTY.get_all(); } \
    inline UpscaleBTreeRange<KEY_TYPE, RECORD_TYPE> get_##PROPERTY##_range(KEY_TYPE key_begin, KEY_TYPE key_end) { return _index_##PROPERTY.get_range(key_begin, key_end); } \

#define INDEXED_STORAGE(CLASS, PROPERTY) \
    class IndexedStorage##__##CLASS##_##PROPERTY : public IndexedStorage<CLASS> { \
    public: \
        inline IndexedStorage##__##CLASS##_##PROPERTY(const std::string& path, const bool PROPERTY##_enable_duplicates=false, const size_t PROPERTY##_cache_size=1<<24, const bool PROPERTY##_autocommit=false) : \
            IndexedStorage<CLASS>(path), \
            _index_##PROPERTY(path + ".index." + #PROPERTY, PROPERTY##_enable_duplicates, PROPERTY##_cache_size, PROPERTY##_autocommit) {} \
        inline void insert(CLASS& record) { \
            _logger.append(record); \
            _index_##PROPERTY.insert(record); \
        } \
        INDEXED_STORAGE_GETTERS(PROPERTY, typeofproperty(CLASS, PROPERTY), CLASS) \
    protected: \
        UPSCALE_BTREE(CLASS, PROPERTY) _index_##PROPERTY; \
    }

#define INDEXED_STORAGE_2(CLASS, PROPERTY1, PROPERTY2) \
    INDEXED_STORAGE(CLASS, PROPERTY1); \
    class IndexedStorage##__##CLASS##_##PROPERTY2##_##PROPERTY2 : public IndexedStorage##__##CLASS##_##PROPERTY1 { \
    public: \
        inline IndexedStorage##__##CLASS##_##PROPERTY2##_##PROPERTY2(const std::string& path, const bool PROPERTY1##_enable_duplicates=false, const bool PROPERTY2##_enable_duplicates=false, const size_t PROPERTY1##_cache_size=1<<24, const size_t PROPERTY2##_cache_size=1<<24, const bool PROPERTY1##_autocommit=false, const bool PROPERTY2##_autocommit=false) : \
            IndexedStorage##__##CLASS##_##PROPERTY1(path, PROPERTY1##_enable_duplicates, PROPERTY1##_cache_size, PROPERTY1##_autocommit), \
            _index_##PROPERTY2(path + ".index." + #PROPERTY2, PROPERTY2##_enable_duplicates, PROPERTY2##_cache_size, PROPERTY2##_autocommit) {} \
        inline void insert(CLASS& record) { \
            _logger.append(record); \
            _index_##PROPERTY1.insert(record); \
            _index_##PROPERTY2.insert(record); \
        } \
        INDEXED_STORAGE_GETTERS(PROPERTY2, typeofproperty(CLASS, PROPERTY2), CLASS) \
    protected: \
        UPSCALE_BTREE(CLASS, PROPERTY2) _index_##PROPERTY2; \
    }


#endif // CTRADING__DB__INDEXEDSTORAGE__HPP
