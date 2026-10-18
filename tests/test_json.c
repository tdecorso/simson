#include "json.h"

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

int main(void) {
    if (test_json_boolean_true()) return 1;
    if (test_json_boolean_false()) return 1;
    if (test_json_null()) return 1;
    if (test_json_number_negative()) return 1;
    if (test_json_number_positive()) return 1;
    if (test_json_number_exponential()) return 1;
    if (test_json_number_zero()) return 1;
    if (test_json_number_not_valid()) return 1;

    return 0;
}
