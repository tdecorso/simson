#ifndef H_simson
#define H_simson

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum simson_type {
    SIMSON_NULL,
    SIMSON_BOOL,
    SIMSON_NUMBER,
    SIMSON_STRING,
    SIMSON_ARRAY,
    SIMSON_OBJECT,
} simson_type_t;

typedef struct simson_text {
    char* data;
    size_t len;
} simson_text_t;

typedef struct simson_value simson_value_t;

typedef struct simson_member {
    simson_text_t* key;
    simson_value_t* value;
} simson_member_t;

typedef struct simson_array {
    simson_value_t** data;
    size_t len;
    size_t cap;
} simson_array_t;

typedef struct simson_object {
    simson_member_t** data;
    size_t len;
    size_t cap;
} simson_object_t;

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

simson_value_t* simson_value_create(simson_type_t type);
void simson_value_destroy(simson_value_t* v);

simson_value_t* simson_value_parse(const uint8_t* buffer, size_t len);

#endif // H_simson
