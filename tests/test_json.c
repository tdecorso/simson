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

int main(void) {
    if (test_json_boolean_true()) return 1;
    if (test_json_boolean_false()) return 1;
    if (test_json_null()) return 1;

    return 0;
}
