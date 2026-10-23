#include "simson.h"
#include <string.h>
#include <stdio.h>

#define TEST(f, title) do {                  \
    printf("%s: ", title);                   \
    if (f()) { printf("FAIL\n"); return 1; } \
    else     { printf("OK\n"); }             \
} while (0)

int test_simson_boolean_true() {
    uint8_t stream[] = "true";
    simson_value_t* v = simson_value_parse(stream, sizeof(stream)-1);
    if (!v) return 1;
    if (!v->as.boolean) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_boolean_false() {
    uint8_t stream[] = "false";
    simson_value_t* v = simson_value_parse(stream, sizeof(stream)-1);
    if (!v) return 1;
    if (v->as.boolean) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_null() {
    uint8_t stream[] = "null";
    simson_value_t* v = simson_value_parse(stream, sizeof(stream)-1);
    if (!v) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_number_negative() {
    uint8_t stream[] = "-12";
    simson_value_t* v = simson_value_parse(stream, sizeof(stream)-1);
    if (!v) return 1;
    if (v->as.number != -12) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_number_error() {
    uint8_t stream[] = "123\00";
    simson_value_t* v = simson_value_parse(stream, sizeof(stream)-1);
    if (v) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_number_exponential() {
    uint8_t stream[] = "12e2";
    simson_value_t* v = simson_value_parse(stream, sizeof(stream)-1);
    if (!v) return 1;
    if (v->as.number != 12e2) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_number_decimal() {
    uint8_t stream[] = "12.25";
    simson_value_t* v = simson_value_parse(stream, sizeof(stream)-1);
    if (!v) return 1;
    if (v->as.number != 12.25) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_number_zero() {
    uint8_t stream[] = "0.125";
    simson_value_t* v = simson_value_parse(stream, sizeof(stream)-1);
    if (!v) return 1;
    if (v->as.number != 0.125) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_number_not_valid() {
    uint8_t stream[] = "0123";
    simson_value_t* v = simson_value_parse(stream, sizeof(stream)-1);
    if (v) return 1;
    return 0;
}

int test_simson_string_ascii() {
    char stream[] = "\"hello\"";
    simson_value_t* v = simson_value_parse((const uint8_t*)stream, sizeof(stream) - 1);
    if (!v) return 1;
    if (strncmp(stream, v->as.string->data, v->as.string->len) != 0) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_string_escaped() {
    char stream[] = "\"hello\\bworld\"";
    simson_value_t* v = simson_value_parse((const uint8_t*)stream, sizeof(stream) - 1);
    if (!v) return 1;
    if (strncmp(stream, v->as.string->data, v->as.string->len) != 0) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_string_utf8() {
    char stream[] = "\"hello😀\"";
    simson_value_t* v = simson_value_parse((const uint8_t*)stream, sizeof(stream) - 1);
    if (!v) return 1;
    if (strncmp(stream, v->as.string->data, v->as.string->len) != 0) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_string_utf16_surrogate_pair() {
    char stream[] = "\"hello\\uD83D\\uDE00\"";
    simson_value_t* v = simson_value_parse((const uint8_t*)stream,sizeof(stream) - 1);
    if (!v) return 1;
    if (strncmp(stream, v->as.string->data, v->as.string->len) != 0) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_array() {
    char stream[] = "[[] ]";
    simson_value_t* v = simson_value_parse((const uint8_t*)stream,sizeof(stream) - 1);
    if (!v) return 1;
    if (v->as.array->len != 1) return 1;
    simson_value_destroy(v);
    return 0;
}

int test_simson_object() {
    char stream[] = "{ \"min\": -1.0e+28, \"max\": 1.0e+28 }";
    simson_value_t* v = simson_value_parse((const uint8_t*)stream,sizeof(stream) - 1);
    if (!v) return 1;
    if (v->as.object->len != 2) return 1;
    simson_value_destroy(v);
    return 0;
}

int main(void) {
    TEST(test_simson_boolean_true,                "Parsing literal 'true'");
    TEST(test_simson_boolean_false,               "Parsing literal 'false'");
    TEST(test_simson_null,                        "Parsing literal 'null'");
    TEST(test_simson_number_negative,             "Parsing valid negative number");
    TEST(test_simson_number_error,                "Parsing invalid number (\00 terminated)");
    TEST(test_simson_number_exponential,          "Parsing valid exp number");
    TEST(test_simson_number_zero,                 "Parsing valid decimal number");
    TEST(test_simson_number_not_valid,            "Parsing invalid number (trailing zeros)");
    TEST(test_simson_string_ascii,                "Parsing ascii string");
    TEST(test_simson_string_escaped,              "Parsing escaped sequence");
    TEST(test_simson_string_utf8,                 "Parsing utf8 string");
    TEST(test_simson_string_utf16_surrogate_pair, "Parsing utf16 surrogate pair");
    TEST(test_simson_array,                       "Parsing array");
    TEST(test_simson_object,                      "Parsing object");

    return 0;
}
