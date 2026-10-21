#include "json.h"
#include <string.h>

int test_json_boolean_true() {
    uint8_t stream[] = "true";
    json_value_t* v = json_value_parse(stream, 4);
    if (!v) return 1;
    if (!v->as.boolean) return 1;
    json_value_destroy(v);
    return 0;
}

int test_json_boolean_false() {
    uint8_t stream[] = "false";
    json_value_t* v = json_value_parse(stream, 5);
    if (!v) return 1;
    if (v->as.boolean) return 1;
    json_value_destroy(v);
    return 0;
}

int test_json_null() {
    uint8_t stream[] = "null";
    json_value_t* v = json_value_parse(stream, 5);
    if (!v) return 1;
    json_value_destroy(v);
    return 0;
}

int test_json_number_negative() {
    uint8_t stream[] = "-12";
    json_value_t* v = json_value_parse(stream, 3);
    if (!v) return 1;
    if (v->as.number != -12) return 1;
    json_value_destroy(v);
    return 0;
}

int test_json_number_positive() {
    uint8_t stream[] = "12";
    json_value_t* v = json_value_parse(stream, 2);
    if (!v) return 1;
    if (v->as.number != 12) return 1;
    json_value_destroy(v);
    return 0;
}

int test_json_number_exponential() {
    uint8_t stream[] = "12e2";
    json_value_t* v = json_value_parse(stream, 4);
    if (!v) return 1;
    if (v->as.number != 12e2) return 1;
    json_value_destroy(v);
    return 0;
}

int test_json_number_decimal() {
    uint8_t stream[] = "12.25";
    json_value_t* v = json_value_parse(stream, 5);
    if (!v) return 1;
    if (v->as.number != 12.25) return 1;
    json_value_destroy(v);
    return 0;
}

int test_json_number_zero() {
    uint8_t stream[] = "0.125";
    json_value_t* v = json_value_parse(stream, 5);
    if (!v) return 1;
    if (v->as.number != 0.125) return 1;
    json_value_destroy(v);
    return 0;
}

int test_json_number_not_valid() {
    uint8_t stream[] = "0123";
    json_value_t* v = json_value_parse(stream, 5);
    if (v) return 1;
    return 0;
}

int test_json_string_ascii() {
    char stream[] = "\"hello\"";
    json_value_t* v = json_value_parse((const uint8_t*)stream, sizeof(stream) - 1);
    if (!v) return 1;
    if (strncmp(stream, v->as.string->data, v->as.string->len) != 0) return 1;
    json_value_destroy(v);
    return 0;
}

int test_json_string_escaped() {
    char stream[] = "\"hello\\bworld\"";
    json_value_t* v = json_value_parse((const uint8_t*)stream, sizeof(stream) - 1);
    if (!v) return 1;
    if (strncmp(stream, v->as.string->data, v->as.string->len) != 0) return 1;
    json_value_destroy(v);
    return 0;
}

int test_json_string_utf8() {
    char stream[] = "\"hello😀\"";
    json_value_t* v = json_value_parse((const uint8_t*)stream, sizeof(stream) - 1);
    if (!v) return 1;
    if (strncmp(stream, v->as.string->data, v->as.string->len) != 0) return 1;
    json_value_destroy(v);
    return 0;
}

int test_json_string_utf16_surrogate_pair() {
    char stream[] = "\"hello\\uD83D\\uDE00\"";
    json_value_t* v = json_value_parse((const uint8_t*)stream,sizeof(stream) - 1);
    if (!v) return 1;
    if (strncmp(stream, v->as.string->data, v->as.string->len) != 0) return 1;
    json_value_destroy(v);
    return 0;
}

int test_json_array() {
    char stream[] = "[1, 2, 3, 4, 5]";
    json_value_t* v = json_value_parse((const uint8_t*)stream,sizeof(stream) - 1);
    if (!v) return 1;
    if (v->as.array->len != 5) return 1;
    json_value_destroy(v);
    return 0;
}

int main(void) {
    if (test_json_boolean_true()) return 1;
    if (test_json_boolean_false()) return 1;
    if (test_json_null()) return 1;
    if (test_json_number_negative()) return 1;
    if (test_json_number_positive()) return 1;
    if (test_json_number_exponential()) return 1;
    if (test_json_number_zero()) return 1;
    if (test_json_number_not_valid()) return 1;
    if (test_json_string_ascii()) return 1;
    if (test_json_string_escaped()) return 1;
    if (test_json_string_utf8()) return 1;
    if (test_json_string_utf16_surrogate_pair()) return 1;
    if (test_json_array()) return 1;

    return 0;
}
