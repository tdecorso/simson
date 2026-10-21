#ifndef H_JSON
#define H_JSON

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum json_type {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT,
} json_type_t;

typedef struct json_text {
    char* data;
    size_t len;
} json_text_t;

typedef struct json_value json_value_t;

typedef struct json_member {
    json_text_t* key;
    json_value_t* value;
} json_member_t;

typedef struct json_array {
    json_value_t** data;
    size_t len;
    size_t cap;
} json_array_t;

typedef struct json_object {
    json_member_t** data;
    size_t len;
    size_t cap;
} json_object_t;

struct json_value {
    json_type_t type;
    union {
        bool boolean;
        double number;
        json_text_t* string;
        json_array_t* array;
        json_object_t* object;
    } as;
};

json_value_t* json_value_create(json_type_t type);
void json_value_destroy(json_value_t* v);

json_value_t* json_value_parse(const uint8_t* buffer, size_t len);

#endif // H_JSON
