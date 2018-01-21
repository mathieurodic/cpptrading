#ifndef CTRADING__DB__UPSCALEBTREE__HPP
#define CTRADING__DB__UPSCALEBTREE__HPP


#include <stdint.h>
#include <string.h>
#include <string>
#include <exception>

#include <ups/upscaledb.h>

#include "exceptions/Exception.hpp"
#include "IO/directories.hpp"


class UpscaleDBException : public DBException {
public:
    template<typename ...Args>
    inline UpscaleDBException(const std::string& method, const ups_status_t& status, Args const&... args) :
        _ups_status(status),
        DBException(std::string("Error ") + std::to_string(status), ups_strerror(status), "when calling '" + method + "' with parameters", args...)
    {}
    inline const ups_status_t& get_status() const {
        return _ups_status;
    }
protected:
    ups_status_t _ups_status;
};


#define UPS_SAFE_CALL(METHOD, ...) { const ups_status_t STATUS = METHOD(__VA_ARGS__); if (STATUS) {throw UpscaleDBException(#METHOD, STATUS, __VA_ARGS__);} }


template <typename key_t, typename record_t>
class UpscaleBTreeCursor {
public:

    inline UpscaleBTreeCursor(const UpscaleBTreeCursor& source) {
        memcpy(this, &source, sizeof(*this));
    }
    inline UpscaleBTreeCursor() : _is_finished(true) {}
    inline UpscaleBTreeCursor(ups_db_t* ups_db, bool is_fullrange, key_t key_begin, key_t key_end) :
        _ups_db(ups_db),
        _ups_cursor(NULL),
        _is_fullrange(is_fullrange),
        _is_reverse(key_end < key_begin),
        _key(key_begin),
        _key_begin(key_begin),
        _key_end(key_end),
        _ups_key({.size=sizeof(key_t), .data=&_key, .flags=UPS_RECORD_USER_ALLOC}),
        _ups_record({.size=sizeof(record_t), .data=&_record, .flags=UPS_RECORD_USER_ALLOC})
    {}

    inline ~UpscaleBTreeCursor() {
        if (_ups_cursor) {
            UPS_SAFE_CALL(ups_cursor_close,
                _ups_cursor);
        }
    }

    // indirection
    inline const record_t& operator * () {
        if (!_is_finished && !_ups_cursor) {
            init_ups_cursor();
        }
        return _record;
    }
    inline const record_t& operator -> () {
        if (!_is_finished && !_ups_cursor) {
            init_ups_cursor();
        }
        return _record;
    }

    // iteration
    inline void operator ++ () {
        if (_is_finished) {
            return;
        }
        try {
            UPS_SAFE_CALL(ups_cursor_move,
                _ups_cursor,
                &_ups_key,
                &_ups_record,
                _is_reverse ? UPS_CURSOR_PREVIOUS : UPS_CURSOR_NEXT
            );
        } catch (const UpscaleDBException& exception) {
            if (exception.get_status() == UPS_KEY_NOT_FOUND || exception.get_status() == UPS_INV_PARAMETER) {
                _is_finished = true;
                return;
            }
        }
        if (!_is_fullrange && (!_is_reverse && _key >= _key_end) || (_is_reverse && _key <= _key_end)) {
            _is_finished = true;
            return;
        }
    }

    inline const bool operator == (const UpscaleBTreeCursor& other) const {
        if (_is_finished && other._is_finished) {
            return true;
        }
        if (_is_finished || other._is_finished) {
            return false;
        }
        return false;
    }
    inline const bool operator != (const UpscaleBTreeCursor& other) const {
        if (_is_finished && other._is_finished) {
            return false;
        }
        if (_is_finished || other._is_finished) {
            return true;
        }
        return true;
    }

private:

    inline void init_ups_cursor() {
        UPS_SAFE_CALL(ups_cursor_create,
            &_ups_cursor,
            _ups_db,
            0, // transaction
            0 // flags (unused)
        );
        try {
            if (_is_fullrange) {
                UPS_SAFE_CALL(ups_cursor_move,
                    _ups_cursor,
                    &_ups_key,
                    &_ups_record,
                    UPS_CURSOR_FIRST
                );
            } else {
                UPS_SAFE_CALL(ups_cursor_find,
                    _ups_cursor,
                    &_ups_key,
                    &_ups_record,
                    _is_reverse ? UPS_FIND_LEQ_MATCH : UPS_FIND_GEQ_MATCH
                );
            }
            _is_finished = false;
        } catch (const UpscaleDBException& exception) {
            if (exception.get_status() == UPS_KEY_NOT_FOUND || exception.get_status() == UPS_INV_PARAMETER || exception.get_status() == UPS_CURSOR_IS_NIL) {
                _is_finished = true;
            }
        }
    }

    bool _is_reverse;
    bool _is_fullrange;
    bool _is_finished;
    key_t _key;
    record_t _record;
    ups_key_t _ups_key;
    ups_record_t _ups_record;
    //
    key_t _key_begin, _key_end;
    ups_db_t* _ups_db;
    ups_cursor_t* _ups_cursor;
};


template <typename key_t, typename record_t>
class UpscaleBTreeRange {
public:
    UpscaleBTreeRange(ups_db_t* ups_db) :
        _ups_db(ups_db),
        _is_fullrange(true)
    {}
    UpscaleBTreeRange(ups_db_t* ups_db, key_t key_begin, key_t key_end) :
        UpscaleBTreeRange(ups_db)
    {
        memcpy(&_key_begin, &key_begin, sizeof(key_t));
        memcpy(&_key_end, &key_end, sizeof(key_t));
        _is_fullrange = false;
    }

    UpscaleBTreeCursor<key_t, record_t> begin() {
        return UpscaleBTreeCursor<key_t, record_t>(_ups_db, _is_fullrange, _key_begin, _key_end);
    }

    const UpscaleBTreeCursor<key_t, record_t>& end() {
        return _cursor_end;
    }

private:

    bool _is_fullrange;
    key_t _key_begin, _key_end;
    ups_db_t* _ups_db;
    //
    static const UpscaleBTreeCursor<key_t, record_t> _cursor_end;
};

template <typename key_t, typename record_t>
const UpscaleBTreeCursor<key_t, record_t> UpscaleBTreeRange<key_t, record_t>::_cursor_end = UpscaleBTreeCursor<key_t, record_t>();


template <typename key_t, size_t key_offset, typename record_t>
class UpscaleBTree {
public:

    UpscaleBTree(const std::string& path, const bool allow_duplicates=false, const size_t cache_size=1<<24, const bool autocommit=false) :
        _path(path),
        _allow_duplicates(allow_duplicates),
        _cache_size(cache_size),
        _autocommit(autocommit)
    {
        // create directory
        make_directory(extract_directory(path));
        // create environment
        static const ups_parameter_t ups_env_parameters[] = {
            {UPS_PARAM_CACHE_SIZE, _cache_size},
            {UPS_PARAM_PAGE_SIZE, 4096},
            // {UPS_PARAM_JOURNAL_COMPRESSION, UPS_COMPRESSOR_SNAPPY},
            {0, 0}
        };
        UPS_SAFE_CALL(ups_env_create,
            &_ups_env,
            _path.c_str(),
            UPS_ENABLE_TRANSACTIONS  |  (_autocommit ? UPS_ENABLE_FSYNC : 0),
            0644,
            ups_env_parameters
        );
        // create database
        static const ups_parameter_t ups_db_parameters[] = {
            {
                UPS_PARAM_KEY_TYPE,
                get_parameter_key_type()
            },
            {
                (get_parameter_key_type() == UPS_TYPE_BINARY) ? UPS_PARAM_KEY_SIZE : 0,
                (get_parameter_key_type() == UPS_TYPE_BINARY) ? sizeof(key_t) : 0
            },
            // {UPS_PARAM_PAGE_SIZE, 4096},
            // {UPS_PARAM_JOURNAL_COMPRESSION, UPS_COMPRESSOR_SNAPPY},
            {0, 0}
        };
        UPS_SAFE_CALL(ups_env_create_db,
            _ups_env,
            &_ups_db,
            1, // name
            (_allow_duplicates ? UPS_ENABLE_DUPLICATE_KEYS : 0),
            ups_db_parameters
        );
    }

    inline ~UpscaleBTree() {
        UPS_SAFE_CALL(ups_env_close,
            _ups_env, UPS_AUTO_CLEANUP);
    }

    inline const std::string& get_path() const {
        return _path;
    }

    inline void insert(record_t& record) {
        char* record_pointer = (char*) &record;
        ups_key_t ups_key = {
            .size = sizeof(key_t),
            .data = record_pointer + key_offset,
            .flags = UPS_RECORD_USER_ALLOC,
        };
        ups_record_t ups_record = {
            .size = sizeof(record_t),
            .data = &record,
            .flags = UPS_RECORD_USER_ALLOC,
        };
        try {
            UPS_SAFE_CALL(ups_db_insert,
                _ups_db,
                NULL, // transaction
                &ups_key,
                &ups_record,
                (_allow_duplicates ? UPS_DUPLICATE : 0) // flags
            );
        } catch (const UpscaleDBException& exception) {
            if (exception.get_status() == UPS_DUPLICATE_KEY) {
                throw DBDuplicateException(* (key_t*) (record_pointer + key_offset));
            } else {
                throw exception;
            }
        }
    }

    inline const record_t get_le(key_t key) {
        return get(key, UPS_FIND_LEQ_MATCH);
    }
    inline const record_t get_ge(key_t key) {
        return get(key, UPS_FIND_GEQ_MATCH);
    }
    inline const record_t get_lt(key_t key) {
        return get(key, UPS_FIND_LT_MATCH);
    }
    inline const record_t get_gt(key_t key) {
        return get(key, UPS_FIND_GT_MATCH);
    }
    inline const record_t get(key_t key, uint32_t flags=0) {
        record_t record;
        ups_key_t ups_key = {
            .size = sizeof(key_t),
            .data = &key,
            .flags = UPS_RECORD_USER_ALLOC,
        };
        ups_record_t ups_record = {
            .size = sizeof(record_t),
            .data = &record,
            .flags = UPS_RECORD_USER_ALLOC,
        };
        try {
            UPS_SAFE_CALL(ups_db_find,
                _ups_db,
                NULL, // transaction
                &ups_key,
                &ups_record,
                flags
            );
        } catch (const UpscaleDBException& exception) {
            if (exception.get_status() == UPS_KEY_NOT_FOUND) {
                throw DBKeyException(key);
            } else {
                throw exception;
            }
        }
        return record;
    }

    inline UpscaleBTreeRange<key_t, record_t> get_all() {
        return UpscaleBTreeRange<key_t, record_t>(_ups_db);
    }
    inline UpscaleBTreeRange<key_t, record_t> get_range(key_t key_begin, key_t key_end) {
        return UpscaleBTreeRange<key_t, record_t>(_ups_db, key_begin, key_end);
    }

    inline const size_t contains(record_t& searched_record) {
        return count(searched_record, true);
    }
    inline const size_t count(const record_t& searched_record, const bool& stop_at_first=false) {
        // extract key
        char* searched_record_pointer = (char*) &searched_record;
        key_t& searched_key = * (key_t*) (searched_record_pointer + key_offset);
        // initialize UPS key & record
        record_t record;
        ups_record_t ups_record = {
            .size = sizeof(record_t),
            .data = &record,
            .flags = UPS_RECORD_USER_ALLOC,
        };
        char* record_pointer = (char*) &record;
        key_t& key = * (key_t*) (record_pointer + key_offset);
        memcpy(&key, &searched_key, sizeof(key_t));
        ups_key_t ups_key = {
            .size = sizeof(key_t),
            .data = &key,
            .flags = UPS_RECORD_USER_ALLOC,
        };
        // create cursor & start it
        ups_cursor_t* ups_cursor;
        UPS_SAFE_CALL(ups_cursor_create,
            &ups_cursor,
            _ups_db,
            0, // transaction
            0 // flags (unused)
        );
        try {
            UPS_SAFE_CALL(ups_cursor_find,
                ups_cursor,
                &ups_key,
                &ups_record,
                UPS_FIND_GEQ_MATCH
            );
        } catch (const UpscaleDBException& exception) {
            if (exception.get_status() == UPS_KEY_NOT_FOUND || exception.get_status() == UPS_INV_PARAMETER || exception.get_status() == UPS_CURSOR_IS_NIL) {
                return 0;
            }
        }
        // test until record is found... or not
        size_t count = 0;
        do {
            if (key > searched_key) {
                return count;
            }
            if (record == searched_record) {
                ++count;
                if (stop_at_first) {
                    return count;
                }
            }
            try {
                UPS_SAFE_CALL(ups_cursor_move,
                    ups_cursor,
                    &ups_key,
                    &ups_record,
                    UPS_CURSOR_NEXT
                );
            } catch (const UpscaleDBException& exception) {
                if (exception.get_status() == UPS_KEY_NOT_FOUND || exception.get_status() == UPS_INV_PARAMETER) {
                    return count;
                }
                throw exception;
            }
        } while (true);
    }
    inline const size_t contains(key_t& searched_key) {
        return count(searched_key, true);
    }
    inline const size_t count(const key_t& searched_key, const bool& stop_at_first=false) {
        // initialize UPS key & record
        record_t record;
        ups_record_t ups_record = {
            .size = sizeof(record_t),
            .data = &record,
            .flags = UPS_RECORD_USER_ALLOC,
        };
        char* record_pointer = (char*) &record;
        key_t& key = * (key_t*) (record_pointer + key_offset);
        memcpy(&key, &searched_key, sizeof(key_t));
        ups_key_t ups_key = {
            .size = sizeof(key_t),
            .data = &key,
            .flags = UPS_RECORD_USER_ALLOC,
        };
        // create cursor & start it
        ups_cursor_t* ups_cursor;
        UPS_SAFE_CALL(ups_cursor_create,
            &ups_cursor,
            _ups_db,
            0, // transaction
            0 // flags (unused)
        );
        try {
            UPS_SAFE_CALL(ups_cursor_find,
                ups_cursor,
                &ups_key,
                &ups_record,
                UPS_FIND_GEQ_MATCH
            );
        } catch (const UpscaleDBException& exception) {
            if (exception.get_status() == UPS_KEY_NOT_FOUND || exception.get_status() == UPS_INV_PARAMETER || exception.get_status() == UPS_CURSOR_IS_NIL) {
                return 0;
            }
        }
        // test until record is found... or not
        size_t count = 0;
        do {
            if (key > searched_key) {
                return count;
            }
            if (key == searched_key) {
                ++count;
                if (stop_at_first) {
                    return count;
                }
            }
            try {
                UPS_SAFE_CALL(ups_cursor_move,
                    ups_cursor,
                    &ups_key,
                    &ups_record,
                    UPS_CURSOR_NEXT
                );
            } catch (const UpscaleDBException& exception) {
                if (exception.get_status() == UPS_KEY_NOT_FOUND || exception.get_status() == UPS_INV_PARAMETER) {
                    return count;
                }
                throw exception;
            }
        } while (true);
    }

protected:

    inline static uint64_t get_parameter_key_type() {
        return get_parameter_key_type(* (key_t*) NULL);
    }
    inline static uint64_t get_parameter_key_type(int8_t&) { return UPS_TYPE_UINT8; }
    inline static uint64_t get_parameter_key_type(uint8_t&) { return UPS_TYPE_UINT8; }
    inline static uint64_t get_parameter_key_type(int16_t&) { return UPS_TYPE_UINT16; }
    inline static uint64_t get_parameter_key_type(uint16_t&) { return UPS_TYPE_UINT16; }
    inline static uint64_t get_parameter_key_type(int32_t&) { return UPS_TYPE_UINT32; }
    inline static uint64_t get_parameter_key_type(uint32_t&) { return UPS_TYPE_UINT32; }
    inline static uint64_t get_parameter_key_type(int64_t&) { return UPS_TYPE_UINT64; }
    inline static uint64_t get_parameter_key_type(uint64_t&) { return UPS_TYPE_UINT64; }
    inline static uint64_t get_parameter_key_type(float&) { return UPS_TYPE_REAL32; }
    inline static uint64_t get_parameter_key_type(double&) { return UPS_TYPE_REAL64; }
    template <typename T> inline static uint64_t get_parameter_key_type(T&) { return UPS_TYPE_BINARY; }

    // parameters
    std::string _path;
    bool _allow_duplicates;
    bool _autocommit;
    size_t _cache_size;
    // internals
    ups_env_t* _ups_env;
    ups_db_t* _ups_db;
};


#ifndef typeof
#define typeof(THING) __typeof(THING)
#endif // typeof

#define UPSCALE_BTREE(CLASS, PROPERTY) UpscaleBTree< \
    typeof( ((CLASS*) NULL)->PROPERTY ), \
    (char*) &(((CLASS*) NULL)->PROPERTY) - (char*) NULL, \
    typeof(CLASS) \
>


#endif // CTRADING__DB__UPSCALEBTREE__HPP
