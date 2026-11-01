/*
 *  ____ ___ __  __ ____   ___  _   _
 * / ___|_ _|  \/  / ___| / _ \| \ | |
 * \___ \| || |\/| \___ \| | | |  \| |
 *  ___) | || |  | |___) | |_| | |\  |
 * |____/___|_|  |_|____/ \___/|_| \_|
 *
 * simson - a small JSON parser and serializer for C
 *
 * Conventions
 * -----------
 * - Functions returning int return 0 on success and non-zero on failure,
 *   unless documented otherwise.
 * - Functions returning pointers return NULL on failure.
 * - Strings are length-delimited and are not NUL-terminated.
 * - String data is stored as raw JSON text without the surrounding quotes.
 *   Escape sequences are preserved verbatim, so callers constructing strings
 *   must provide valid, already-escaped JSON string content.
 * - Values are owned by the caller. Container mutators take ownership of
 *   inserted values on success; on failure the caller keeps ownership.
 *   Removed values are detached and returned to the caller.
 */

#ifndef SIMSON_H
#define SIMSON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
 * The JSON value types.
 */
typedef enum simson_type {
    SIMSON_NULL,    /* null */
    SIMSON_BOOL,    /* true or false */
    SIMSON_NUMBER,  /* double-precision number */
    SIMSON_STRING,  /* length-delimited raw JSON text */
    SIMSON_ARRAY,   /* ordered list of values */
    SIMSON_OBJECT,  /* insertion-ordered key/value members */
} simson_type_t;

/*
 * A length-delimited byte string. `data` is not NUL-terminated.
 *
 * String values store the raw JSON text between the surrounding quotes, with
 * escape sequences preserved exactly as written.
 */
typedef struct simson_text {
    char* data;
    size_t len;
} simson_text_t;

typedef struct simson_value simson_value_t;

/*
 * A single key/value member of an object. Both `key` and `value` are owned by
 * the containing object.
 */
typedef struct simson_member {
    simson_text_t* key;
    simson_value_t* value;
} simson_member_t;

/*
 * An ordered array of values. Elements are owned by the array.
 */
typedef struct simson_array {
    simson_value_t** data;
    size_t len;
    size_t cap;
} simson_array_t;

/*
 * An insertion-ordered collection of key/value members. Members are owned by
 * the object.
 */
typedef struct simson_object {
    simson_member_t** data;
    size_t len;
    size_t cap;
} simson_object_t;

/*
 * A JSON value. The active member of `as` is determined by `type`.
 *
 * Prefer the typed constructors and accessors below over manipulating this
 * struct directly.
 */
struct simson_value {
    simson_type_t type;
    union {
        bool boolean;
        double number;
        simson_text_t* string;
        simson_array_t* array;
        simson_object_t* object;
    } as;
};

/* ========================================================================== */
/* Lifetime                                                                   */
/* ========================================================================== */

/*
 * Create an empty value of the given type.
 *
 * This is a low-level constructor; the typed constructors below are usually
 * more convenient. Strings created this way have no text, and arrays and
 * objects are allocated lazily by their mutators.
 *
 * Returns NULL on allocation failure.
 */
simson_value_t* simson_value_create(simson_type_t type);

/*
 * Recursively destroy a value, including all of its children.
 * Safe to call with NULL.
 */
void simson_value_destroy(simson_value_t* v);

/* ========================================================================== */
/* Construction                                                               */
/* ========================================================================== */

/*
 * Typed constructors. Each returns an owned value, or NULL on allocation
 * failure.
 *
 * simson_value_string copies `len` bytes of raw JSON string content. The text
 * must already be escaped and must not include the surrounding quotes. A NULL
 * text pointer is allowed only when len is zero.
 */
simson_value_t* simson_value_null(void);
simson_value_t* simson_value_bool(bool value);
simson_value_t* simson_value_number(double value);
simson_value_t* simson_value_string(const char* text, size_t len);
simson_value_t* simson_value_array(void);
simson_value_t* simson_value_object(void);

/* ========================================================================== */
/* Value access                                                               */
/* ========================================================================== */

/*
 * Read a value's type or scalar contents.
 *
 * Each function returns 0 on success and writes through `out`. A NULL argument
 * or a type mismatch returns non-zero.
 *
 * simson_value_get_string returns a borrowed pointer to length-delimited raw
 * JSON text. The pointer remains valid until the value is destroyed or
 * modified, and is not NUL-terminated.
 */
int simson_value_type(const simson_value_t* v, simson_type_t* out);
int simson_value_get_bool(const simson_value_t* v, bool* out);
int simson_value_get_number(const simson_value_t* v, double* out);
int simson_value_get_string(const simson_value_t* v, const char** data, size_t* len);

/* ========================================================================== */
/* Arrays                                                                     */
/* ========================================================================== */

/*
 * Array access and mutation.
 *
 * All functions require an array value. simson_array_insert accepts any index
 * from 0 through the current length; the other indexed operations require
 * index < length. Mutators take ownership of `value` on success.
 */
int simson_array_len(const simson_value_t* v, size_t* len);

/* Borrow the element at `index`. */
int simson_array_get(const simson_value_t* v, size_t index, simson_value_t** out);

/* Append a value, taking ownership on success. */
int simson_array_append(simson_value_t* v, simson_value_t* value);

/* Insert at `index`, shifting later elements right. */
int simson_array_insert(simson_value_t* v, size_t index, simson_value_t* value);

/* Replace the element at `index`; the old value is destroyed. */
int simson_array_set(simson_value_t* v, size_t index, simson_value_t* value);

/*
 * Detach and return the element at `index`.
 * The caller must destroy the returned value.
 */
simson_value_t* simson_array_remove(simson_value_t* v, size_t index);

/* ========================================================================== */
/* Objects                                                                    */
/* ========================================================================== */

/*
 * Object access and mutation.
 *
 * Keys are length-delimited raw JSON text without the surrounding quotes.
 * Mutations keep keys unique: setting an existing key replaces its value in
 * place, preserving insertion order. Iteration order is insertion order.
 */
int simson_object_len(const simson_value_t* v, size_t* len);

/*
 * Borrow the value stored under `key`.
 * Returns non-zero if the key is not present.
 */
int simson_object_get(const simson_value_t* v, const char* key, size_t key_len, simson_value_t** out);

/*
 * Borrow the key at `index`.
 *
 * The returned pointer remains valid until the object is destroyed or
 * modified, and is not NUL-terminated.
 */
int simson_object_key(const simson_value_t* v, size_t index, const char** key, size_t* key_len);

/*
 * Set `key` to `value`, taking ownership of `value` on success.
 * If the key already exists, its old value is destroyed and replaced.
 */
int simson_object_set(simson_value_t* v, const char* key, size_t key_len, simson_value_t* value);

/*
 * Detach and return the value stored under `key`.
 * Returns NULL if the key is not present. The caller must destroy the value.
 */
simson_value_t* simson_object_remove(simson_value_t* v, const char* key, size_t key_len);

/* ========================================================================== */
/* Parsing and serialization                                                  */
/* ========================================================================== */

/*
 * Parse a JSON document from `len` bytes at `buffer`.
 *
 * The buffer does not need to be NUL-terminated. Leading and trailing JSON
 * whitespace is allowed. Returns an owned value, or NULL if the input is
 * invalid or has trailing non-whitespace content.
 */
simson_value_t* simson_value_parse(const char* buffer, size_t len);

/*
 * Serialize `v` as JSON.
 *
 * On success, returns 0 and stores a malloc'd, NUL-terminated buffer in
 * `*buffer` and its byte length (excluding the NUL) in `*len`. The caller owns
 * the buffer and must free it. On failure, returns non-zero and leaves
 * `*buffer` NULL and `*len` zero.
 *
 * When `pretty` is true, output uses two-space indentation and does not end
 * with a newline. Non-finite numbers cannot be represented and cause failure.
 * Strings are emitted by wrapping their stored raw JSON text in quotes, so
 * string values must contain valid escaped content.
 */
int simson_value_serialize(simson_value_t* v, bool pretty, char** buffer, size_t* len);

#endif /* SIMSON_H */
