#ifndef CTRADING__DB__UPSCALEBTREE__HPP
#define CTRADING__DB__UPSCALEBTREE__HPP


#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <exception>

#include <ups/upscaledb.h>

#include "exceptions/Exception.hpp"
#include "models/Timestamp.hpp"
#include "IO/directories.hpp"
#include "range/Range.hpp"


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


#define UPS_SAFE_CALL(METHOD, ...) { \
    ups_status_t STATUS = UPS_SUCCESS; \
    while (true) { \
        STATUS = METHOD(__VA_ARGS__); \
        if (STATUS == UPS_TXN_CONFLICT) { \
            usleep(1); \
        } else { \
            break; \
        } \
    } \
    if (STATUS != UPS_SUCCESS) { \
        throw UpscaleDBException(#METHOD, STATUS, __VA_ARGS__); \
    } \
}


template <typename key_t, typename record_t>
class UpscaleBTreeRangeData : public RangeData<record_t> {
public:

    inline UpscaleBTreeRangeData(ups_db_t* ups_db, ups_txn_t* ups_txn) :
        _ups_db(ups_db),
        _ups_txn(ups_txn),
        _is_fullrange(true),
        _is_point(false),
        _is_reverse(false),
        _ups_key({.size=sizeof(key_t), .data=&_key, .flags=UPS_RECORD_USER_ALLOC}),
        _ups_record({.size=sizeof(record_t), .data=&_record, .flags=UPS_RECORD_USER_ALLOC})
        {}
    inline UpscaleBTreeRangeData(ups_db_t* ups_db, ups_txn_t* ups_txn, const key_t& key_target) :
        UpscaleBTreeRangeData(ups_db, ups_txn, key_target)
        {}
    inline UpscaleBTreeRangeData(ups_db_t* ups_db, ups_txn_t* ups_txn, const key_t& key_begin, const key_t& key_end) :
        _ups_db(ups_db),
        _ups_txn(ups_txn),
        _is_fullrange(false),
        _is_point(key_begin == key_end),
        _is_reverse(key_end < key_begin),
        _key(key_begin),
        _key_begin(key_begin),
        _key_end(key_end),
        _ups_key({.size=sizeof(key_t), .data=&_key, .flags=UPS_RECORD_USER_ALLOC}),
        _ups_record({.size=sizeof(record_t), .data=&_record, .flags=UPS_RECORD_USER_ALLOC})
        {}

    inline ~UpscaleBTreeRangeData() {
        if (_ups_cursor) {
            UPS_SAFE_CALL(ups_cursor_close,
                _ups_cursor);
            _ups_cursor = NULL;
        }
    }

    virtual const bool init(record_t*& value) {
        value = &_record;
        UPS_SAFE_CALL(ups_cursor_create,
            &_ups_cursor,
            _ups_db,
            _ups_txn, // transaction
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
                std::cout << _key << '\n';
                UPS_SAFE_CALL(ups_cursor_find,
                    _ups_cursor,
                    &_ups_key,
                    &_ups_record,
                    _is_point ? UPS_FIND_EQ_MATCH : (_is_reverse ? UPS_FIND_GEQ_MATCH : UPS_FIND_GT_MATCH)
                );
            }
            return true;
        } catch (const UpscaleDBException& exception) {
            if (exception.get_status() == UPS_KEY_NOT_FOUND || exception.get_status() == UPS_INV_PARAMETER || exception.get_status() == UPS_CURSOR_IS_NIL) {
                return false;
            }
        }
    }
    virtual const bool next(record_t*& value) {
        try {
            UPS_SAFE_CALL(ups_cursor_move,
                _ups_cursor,
                &_ups_key,
                &_ups_record,
                _is_reverse ? UPS_CURSOR_PREVIOUS : UPS_CURSOR_NEXT
            );
        } catch (const UpscaleDBException& exception) {
            if (exception.get_status() == UPS_KEY_NOT_FOUND || exception.get_status() == UPS_INV_PARAMETER) {
                return false;
            } else {
                throw exception;
            }
        }
        if (!_is_fullrange && (!_is_reverse && _key > _key_end) || (_is_reverse && _key < _key_end)) {
            return false;
        }
        return true;
    }

private:

    bool _is_reverse;
    bool _is_fullrange;
    bool _is_point;
    key_t _key;
    record_t _record;
    ups_key_t _ups_key;
    ups_record_t _ups_record;
    //
    key_t _key_begin, _key_end;
    ups_db_t* _ups_db;
    ups_txn_t* _ups_txn;
    ups_cursor_t* _ups_cursor;
};


template <typename key_t, typename record_t>
class UpscaleBTreeRange : public Range<record_t> {
public:

    UpscaleBTreeRange(ups_db_t* ups_db, ups_txn_t* ups_txn) :
        Range<record_t>(new UpscaleBTreeRangeData<key_t, record_t>(ups_db, ups_txn)) {}

    template <typename key_begin_t, typename key_end_t>
    UpscaleBTreeRange(ups_db_t* ups_db, ups_txn_t* ups_txn, const key_begin_t& key_target) :
        Range<record_t>(new UpscaleBTreeRangeData<key_t, record_t>(ups_db, ups_txn, key_target)) {}

    template <typename key_begin_t, typename key_end_t>
    UpscaleBTreeRange(ups_db_t* ups_db, ups_txn_t* ups_txn, const key_begin_t& key_begin, const key_end_t& key_end) :
        Range<record_t>(new UpscaleBTreeRangeData<key_t, record_t>(ups_db, ups_txn, key_begin, key_end)) {}
};


template <typename key_t, typename record_t>
class UpscaleBTree {
public:

    UpscaleBTree(const std::string& path, const bool allow_duplicates=true, const size_t cache_size=1<<24, const bool autocommit=false) :
        _path(path),
        _allow_duplicates(allow_duplicates),
        _cache_size(cache_size),
        _autocommit(autocommit)
    {
        // create directory
        make_directory(extract_directory(path));
        // create environment
        ups_parameter_t ups_env_parameters[] = {
            {UPS_PARAM_CACHE_SIZE, _cache_size},
            {0, 0},
            {0, 0},
        };
        try {
            UPS_SAFE_CALL(ups_env_open,
                &_ups_env,
                _path.c_str(),
                UPS_AUTO_RECOVERY | UPS_ENABLE_TRANSACTIONS | (_autocommit ? UPS_ENABLE_FSYNC : 0),
                ups_env_parameters
            );
        } catch (const UpscaleDBException& exception) {
            if (exception.get_status() == UPS_FILE_NOT_FOUND) {
                ups_env_parameters[1] = {UPS_PARAM_PAGE_SIZE, 4096};
                UPS_SAFE_CALL(ups_env_create,
                    &_ups_env,
                    _path.c_str(),
                    UPS_AUTO_RECOVERY | UPS_ENABLE_TRANSACTIONS | (_autocommit ? UPS_ENABLE_FSYNC : 0),
                    0644,
                    ups_env_parameters
                );
            } else {
                throw exception;
            }
        }
        // create database
        ups_parameter_t ups_db_parameters[] = {
            {
                UPS_PARAM_KEY_TYPE,
                get_parameter_key_type()
            },
            {
                (get_parameter_key_type() == UPS_TYPE_BINARY) ? UPS_PARAM_KEY_SIZE : 0,
                (get_parameter_key_type() == UPS_TYPE_BINARY) ? sizeof(key_t) : 0
            },
            {0, 0}
        };
        try {
            UPS_SAFE_CALL(ups_env_create_db,
                _ups_env,
                &_ups_db,
                1, // name
                (_allow_duplicates ? UPS_ENABLE_DUPLICATE_KEYS : 0),
                ups_db_parameters
            );
        } catch (const UpscaleDBException& exception) {
            if (exception.get_status() == UPS_DATABASE_ALREADY_EXISTS) {
                ups_db_parameters[0] = {0, 0};
                UPS_SAFE_CALL(ups_env_open_db,
                    _ups_env,
                    &_ups_db,
                    1, // name
                    0, // flags
                    ups_db_parameters
                );
            } else {
                throw exception;
            }
        }
        // create transaction
        UPS_SAFE_CALL(ups_txn_begin,
            &_ups_read_txn,
            _ups_env,
            "READER",
            NULL,
            0
        )
    }

    inline ~UpscaleBTree() {
        ups_txn_abort(
            _ups_read_txn,
            0
        );
        UPS_SAFE_CALL(ups_env_close,
            _ups_env, UPS_AUTO_CLEANUP);
        _ups_env = NULL;
    }

    inline const std::string& get_path() const {
        return _path;
    }

    inline void insert(key_t& key, record_t& record) {
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

        ups_txn_t* ups_write_txn;
        UPS_SAFE_CALL(ups_txn_begin,
            &ups_write_txn,
            _ups_env,
            "WRITER",
            NULL,
            0
        )
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
                throw DBDuplicateException(key);
            } else {
                throw exception;
            }
        }
        UPS_SAFE_CALL(ups_txn_commit,
            ups_write_txn,
            0
        )
    }

    inline UpscaleBTreeRange<key_t, record_t> get() {
        return UpscaleBTreeRange<key_t, record_t>(_ups_db, _ups_read_txn);
    }
    inline UpscaleBTreeRange<key_t, record_t> get(key_t key_target) {
        return UpscaleBTreeRange<key_t, record_t>(_ups_db, _ups_read_txn, key_target, key_target);
    }
    inline UpscaleBTreeRange<key_t, record_t> get(key_t key_begin, key_t key_end) {
        return UpscaleBTreeRange<key_t, record_t>(_ups_db, _ups_read_txn, key_begin, key_end);
    }

    inline const bool contains(record_t& searched_record) {
        return count(searched_record, true);
    }
    inline const size_t count(const key_t searched_key, const record_t& searched_record, const bool& stop_at_first=false) {
        // extract key
        char* searched_record_pointer = (char*) &searched_record;
        // initialize UPS key & record
        record_t record;
        ups_record_t ups_record = {
            .size = sizeof(record_t),
            .data = &record,
            .flags = UPS_RECORD_USER_ALLOC,
        };
        char* record_pointer = (char*) &record;
        key_t key = searched_key;
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
            _ups_read_txn,
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
            UPS_SAFE_CALL(ups_cursor_close,
                ups_cursor
            );
            if (exception.get_status() == UPS_KEY_NOT_FOUND || exception.get_status() == UPS_INV_PARAMETER || exception.get_status() == UPS_CURSOR_IS_NIL) {
                return 0;
            } else {
                throw exception;
            }
        }
        // test until record is found... or not
        size_t count = 0;
        do {
            if (key > searched_key) {
                break;
            }
            if (record == searched_record) {
                ++count;
                if (stop_at_first) {
                    break;
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
                    break;
                }
                UPS_SAFE_CALL(ups_cursor_close,
                    ups_cursor
                );
                throw exception;
            }
        } while (true);
        UPS_SAFE_CALL(ups_cursor_close,
            ups_cursor
        );
        return count;
    }
    inline const bool contains(key_t& searched_key) {
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
        key_t key = searched_key;
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
            _ups_read_txn,
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
            UPS_SAFE_CALL(ups_cursor_close,
                ups_cursor
            );
            if (exception.get_status() == UPS_KEY_NOT_FOUND || exception.get_status() == UPS_INV_PARAMETER || exception.get_status() == UPS_CURSOR_IS_NIL) {
                return 0;
            } else {
                throw exception;
            }
        }
        // test until record is found... or not
        size_t count = 0;
        do {
            if (key > searched_key) {
                break;
            }
            if (key == searched_key) {
                ++count;
                if (stop_at_first) {
                    break;
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
                    break;
                }
                UPS_SAFE_CALL(ups_cursor_close,
                    ups_cursor
                );
                throw exception;
            }
        } while (true);
        UPS_SAFE_CALL(ups_cursor_close,
            ups_cursor
        );
        return count;
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
    inline static uint64_t get_parameter_key_type(Timestamp&) { return UPS_TYPE_REAL64; }
    template <typename T> inline static uint64_t get_parameter_key_type(T&) { return UPS_TYPE_BINARY; }

    // parameters
    std::string _path;
    bool _allow_duplicates;
    bool _autocommit;
    size_t _cache_size;
    // internals
    ups_env_t* _ups_env;
    ups_db_t* _ups_db;
    ups_txn_t* _ups_read_txn;
};


#ifndef typeof
#define typeof(THING) __typeof(THING)
#endif // typeof


#endif // CTRADING__DB__UPSCALEBTREE__HPP
