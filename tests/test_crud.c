#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simson.h"

static int failures = 0;

#define CHECK(cond) do {                                                  \
    if (!(cond)) {                                                        \
        printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);            \
        failures++;                                                       \
    }                                                                     \
} while (0)

static void check_number(simson_value_t* v, double expected) {
    double actual = 0;
    CHECK(simson_value_get_number(v, &actual) == 0);
    CHECK(actual == expected);
}

int main(void) {
    simson_type_t type;
    bool boolean = false;
    double number = 0;

    simson_value_t* nullv = simson_value_null();
    CHECK(nullv != NULL);
    CHECK(simson_value_type(nullv, &type) == 0 && type == SIMSON_NULL);
    CHECK(simson_value_get_bool(nullv, &boolean) == 1);
    simson_value_destroy(nullv);

    simson_value_t* boolv = simson_value_bool(true);
    CHECK(boolv != NULL);
    CHECK(simson_value_get_bool(boolv, &boolean) == 0 && boolean == true);
    CHECK(simson_value_get_number(boolv, &number) == 1);
    simson_value_destroy(boolv);

    simson_value_t* numv = simson_value_number(1.5);
    CHECK(numv != NULL);
    CHECK(simson_value_get_number(numv, &number) == 0 && number == 1.5);
    simson_value_destroy(numv);

    simson_value_t* strv = simson_value_string("a\\nb", 4);
    CHECK(strv != NULL);
    const char* data = NULL;
    size_t text_len = 0;
    CHECK(simson_value_get_string(strv, &data, &text_len) == 0);
    CHECK(text_len == 4 && memcmp(data, "a\\nb", 4) == 0);
    simson_value_destroy(strv);

    simson_value_t* empty_str = simson_value_string(NULL, 0);
    CHECK(empty_str != NULL);
    CHECK(simson_value_get_string(empty_str, &data, &text_len) == 0);
    CHECK(text_len == 0);
    simson_value_destroy(empty_str);
    CHECK(simson_value_string(NULL, 1) == NULL);

    simson_value_t* arr = simson_value_array();
    size_t len = 1;
    CHECK(simson_array_len(arr, &len) == 0 && len == 0);
    CHECK(simson_array_append(arr, simson_value_number(1)) == 0);
    CHECK(simson_array_append(arr, simson_value_number(2)) == 0);
    CHECK(simson_array_insert(arr, 1, simson_value_number(3)) == 0);
    CHECK(simson_array_len(arr, &len) == 0 && len == 3);

    simson_value_t* item = NULL;
    CHECK(simson_array_get(arr, 0, &item) == 0);
    check_number(item, 1);
    CHECK(simson_array_get(arr, 1, &item) == 0);
    check_number(item, 3);
    CHECK(simson_array_get(arr, 2, &item) == 0);
    check_number(item, 2);

    CHECK(simson_array_set(arr, 0, simson_value_number(9)) == 0);
    simson_value_t* removed = simson_array_remove(arr, 1);
    CHECK(removed != NULL);
    check_number(removed, 3);
    simson_value_destroy(removed);
    CHECK(simson_array_len(arr, &len) == 0 && len == 2);
    CHECK(simson_array_get(arr, 0, &item) == 0);
    check_number(item, 9);
    CHECK(simson_array_get(arr, 1, &item) == 0);
    check_number(item, 2);

    CHECK(simson_array_append(arr, NULL) == 1);
    simson_value_t* temp = simson_value_number(1);
    CHECK(simson_array_insert(arr, 99, temp) == 1);
    simson_value_destroy(temp);
    temp = simson_value_number(1);
    CHECK(simson_array_set(arr, 99, temp) == 1);
    simson_value_destroy(temp);
    CHECK(simson_array_get(arr, 99, &item) == 1 && item == NULL);
    CHECK(simson_array_remove(arr, 99) == NULL);

    simson_value_t* lazy_array = simson_value_create(SIMSON_ARRAY);
    CHECK(simson_array_append(lazy_array, simson_value_bool(false)) == 0);
    simson_value_destroy(lazy_array);

    simson_value_t* wrong = simson_value_object();
    CHECK(simson_array_len(wrong, &len) == 1);
    simson_value_destroy(wrong);
    simson_value_destroy(arr);

    simson_value_t* obj = simson_value_object();
    CHECK(simson_object_len(obj, &len) == 0 && len == 0);
    CHECK(simson_object_set(obj, "a", 1, simson_value_number(1)) == 0);
    CHECK(simson_object_set(obj, "b", 1, simson_value_array()) == 0);
    CHECK(simson_object_set(obj, "a", 1, simson_value_number(2)) == 0);
    CHECK(simson_object_len(obj, &len) == 0 && len == 2);

    CHECK(simson_object_get(obj, "a", 1, &item) == 0);
    check_number(item, 2);
    CHECK(simson_object_get(obj, "b", 1, &item) == 0);
    CHECK(simson_value_type(item, &type) == 0 && type == SIMSON_ARRAY);
    CHECK(simson_object_get(obj, "missing", 7, &item) == 1 && item == NULL);

    const char* key = NULL;
    size_t key_len = 0;
    CHECK(simson_object_key(obj, 0, &key, &key_len) == 0);
    CHECK(key_len == 1 && memcmp(key, "a", 1) == 0);
    CHECK(simson_object_key(obj, 1, &key, &key_len) == 0);
    CHECK(key_len == 1 && memcmp(key, "b", 1) == 0);
    CHECK(simson_object_key(obj, 2, &key, &key_len) == 1);

    removed = simson_object_remove(obj, "a", 1);
    CHECK(removed != NULL);
    check_number(removed, 2);
    simson_value_destroy(removed);
    CHECK(simson_object_len(obj, &len) == 0 && len == 1);
    CHECK(simson_object_remove(obj, "a", 1) == NULL);

    CHECK(simson_object_set(obj, "", 0, simson_value_bool(true)) == 0);
    CHECK(simson_object_get(obj, "", 0, &item) == 0);
    CHECK(simson_value_get_bool(item, &boolean) == 0 && boolean == true);
    removed = simson_object_remove(obj, "", 0);
    CHECK(removed != NULL);
    simson_value_destroy(removed);

    temp = simson_value_bool(true);
    CHECK(simson_object_set(obj, NULL, 0, temp) == 1);
    simson_value_destroy(temp);
    CHECK(simson_object_set(obj, "x", 1, NULL) == 1);
    CHECK(simson_object_get(obj, NULL, 0, &item) == 1);
    CHECK(simson_object_get(obj, "x", 1, &item) == 1 && item == NULL);
    CHECK(simson_object_remove(obj, NULL, 0) == NULL);

    wrong = simson_value_array();
    CHECK(simson_object_len(wrong, &len) == 1);
    simson_value_destroy(wrong);
    simson_value_destroy(obj);

    const char* json = "{\"name\":\"x\",\"items\":[1,2]}";
    simson_error_t error;
    simson_value_t* root = simson_value_parse(json, strlen(json), &error);
    CHECK(root != NULL);
    simson_value_t* items = NULL;
    CHECK(simson_object_get(root, "items", 5, &items) == 0);
    CHECK(simson_array_append(items, simson_value_number(3)) == 0);
    CHECK(simson_object_set(root, "name", 4, simson_value_string("y", 1)) == 0);
    CHECK(simson_object_set(root, "active", 6, simson_value_bool(true)) == 0);

    char* out = NULL;
    size_t out_len = 0;
    CHECK(simson_value_serialize(root, false, &out, &out_len) == 0);
    const char* expected = "{\"name\":\"y\",\"items\":[1,2,3],\"active\":true}";
    CHECK(out != NULL && out_len == strlen(expected) && memcmp(out, expected, out_len) == 0);
    free(out);
    simson_value_destroy(root);

    if (failures > 0) {
        printf("%d failure(s)\n", failures);
        return 1;
    }
    printf("All CRUD tests passed\n");
    return 0;
}
