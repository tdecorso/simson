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

simson_value_t* simson_value_null(void);
simson_value_t* simson_value_bool(bool value);
simson_value_t* simson_value_number(double value);
simson_value_t* simson_value_string(const char* text, size_t len);
simson_value_t* simson_value_array(void);
simson_value_t* simson_value_object(void);

int simson_value_type(const simson_value_t* v, simson_type_t* out);
int simson_value_get_bool(const simson_value_t* v, bool* out);
int simson_value_get_number(const simson_value_t* v, double* out);
int simson_value_get_string(const simson_value_t* v, const char** data, size_t* len);

int simson_array_len(const simson_value_t* v, size_t* len);
int simson_array_get(const simson_value_t* v, size_t index, simson_value_t** out);
int simson_array_append(simson_value_t* v, simson_value_t* value);
int simson_array_insert(simson_value_t* v, size_t index, simson_value_t* value);
int simson_array_set(simson_value_t* v, size_t index, simson_value_t* value);
simson_value_t* simson_array_remove(simson_value_t* v, size_t index);

int simson_object_len(const simson_value_t* v, size_t* len);
int simson_object_get(const simson_value_t* v, const char* key, size_t key_len, simson_value_t** out);
int simson_object_key(const simson_value_t* v, size_t index, const char** key, size_t* key_len);
int simson_object_set(simson_value_t* v, const char* key, size_t key_len, simson_value_t* value);
simson_value_t* simson_object_remove(simson_value_t* v, const char* key, size_t key_len);

simson_value_t* simson_value_parse(const char* buffer, size_t len);

int simson_value_serialize(simson_value_t* v, bool pretty, char** buffer, size_t* len);

#endif // H_simson
