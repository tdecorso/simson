#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simson.h"

static int failures = 0;

static simson_value_t* parse(const char* input) {
    simson_error_t error;
    return simson_value_parse(input, strlen(input), &error);
}

static void expect_serialize(const char* label, const char* input, bool pretty, const char* expected) {
    simson_value_t* v = parse(input);
    if (!v) {
        printf("FAIL %s: parse failed\n", label);
        failures++;
        return;
    }
    char* buffer = NULL;
    size_t len = 0;
    if (simson_value_serialize(v, pretty, &buffer, &len) != 0) {
        printf("FAIL %s: serialize returned an error\n", label);
        failures++;
        simson_value_destroy(v);
        return;
    }
    if (!buffer) {
        printf("FAIL %s: buffer is NULL\n", label);
        failures++;
        simson_value_destroy(v);
        return;
    }
    size_t expected_len = strlen(expected);
    if (len != expected_len || memcmp(buffer, expected, expected_len) != 0) {
        printf("FAIL %s:\n  expected: %s\n  actual:   %s\n", label, expected, buffer);
        failures++;
    }
    if (buffer[len] != '\0') {
        printf("FAIL %s: buffer is not NUL-terminated\n", label);
        failures++;
    }
    free(buffer);
    simson_value_destroy(v);
}

static void expect_roundtrip(const char* label, const char* input, bool pretty) {
    simson_value_t* v = parse(input);
    if (!v) {
        printf("FAIL %s: parse failed\n", label);
        failures++;
        return;
    }
    char* out1 = NULL;
    size_t len1 = 0;
    if (simson_value_serialize(v, pretty, &out1, &len1) != 0) {
        printf("FAIL %s: first serialize failed\n", label);
        failures++;
        simson_value_destroy(v);
        return;
    }
    simson_error_t error;
    simson_value_t* v2 = simson_value_parse(out1, len1, &error);
    if (!v2) {
        printf("FAIL %s: reparse failed for: %s\n", label, out1);
        failures++;
        free(out1);
        simson_value_destroy(v);
        return;
    }
    char* out2 = NULL;
    size_t len2 = 0;
    if (simson_value_serialize(v2, pretty, &out2, &len2) != 0) {
        printf("FAIL %s: second serialize failed\n", label);
        failures++;
        free(out1);
        simson_value_destroy(v2);
        simson_value_destroy(v);
        return;
    }
    if (len1 != len2 || memcmp(out1, out2, len1) != 0) {
        printf("FAIL %s: round-trip mismatch:\n  first:  %s\n  second: %s\n", label, out1, out2);
        failures++;
    }
    free(out1);
    free(out2);
    simson_value_destroy(v2);
    simson_value_destroy(v);
}

static void expect_error(const char* label, simson_value_t* v) {
    char* buffer = NULL;
    size_t len = 0;
    if (simson_value_serialize(v, false, &buffer, &len) == 0) {
        printf("FAIL %s: expected an error\n", label);
        failures++;
    }
    if (buffer != NULL) {
        printf("FAIL %s: buffer should stay NULL on error\n", label);
        failures++;
    }
    free(buffer);
}

static void expect_parse_failure(const char* label, const char* input) {
    simson_value_t* v = parse(input);
    if (v) {
        printf("FAIL %s: expected parse failure\n", label);
        failures++;
        simson_value_destroy(v);
    }
}

static void expect_locale_numbers(void) {
    const char* original = setlocale(LC_NUMERIC, NULL);
    char saved[128];
    saved[0] = '\0';
    if (original) {
        strncpy(saved, original, sizeof(saved) - 1);
        saved[sizeof(saved) - 1] = '\0';
    }

    static const char* candidates[] = {
        "de_DE.UTF-8", "de_DE.utf8", "de_DE",
        "fr_FR.UTF-8", "fr_FR.utf8", "fr_FR",
        "es_ES.UTF-8", "es_ES.utf8", "es_ES",
        "it_IT.UTF-8", "it_IT.utf8", "it_IT",
        NULL
    };

    bool tested = false;
    for (size_t i = 0; candidates[i]; i++) {
        if (!setlocale(LC_NUMERIC, candidates[i])) continue;
        const char* decimal = localeconv()->decimal_point;
        if (!decimal || strcmp(decimal, ".") == 0) continue;

        simson_value_t* v = parse("1.5");
        if (!v) {
            printf("FAIL locale %s: parse failed\n", candidates[i]);
            failures++;
        } else {
            double number = 0;
            if (simson_value_get_number(v, &number) != 0 || number != 1.5) {
                printf("FAIL locale %s: parsed number is not 1.5\n", candidates[i]);
                failures++;
            }
            simson_value_destroy(v);
        }

        v = simson_value_number(1.5);
        char* buffer = NULL;
        size_t len = 0;
        if (!v || simson_value_serialize(v, false, &buffer, &len) != 0) {
            printf("FAIL locale %s: serialize failed\n", candidates[i]);
            failures++;
        } else if (strcmp(buffer, "1.5") != 0) {
            printf("FAIL locale %s: serialized %s instead of 1.5\n", candidates[i], buffer);
            failures++;
        }
        free(buffer);
        simson_value_destroy(v);
        tested = true;
        break;
    }

    setlocale(LC_NUMERIC, saved[0] ? saved : "C");
    if (!tested) {
        printf("SKIP locale number test: no comma-decimal locale available\n");
    }
}

int main(void) {
    expect_serialize("null", "null", false, "null");
    expect_serialize("true", "true", false, "true");
    expect_serialize("false", "false", false, "false");
    expect_serialize("integer", "42", false, "42");
    expect_serialize("decimal", "123.456789", false, "123.456789");
    expect_serialize("short decimal", "0.1", false, "0.1");
    expect_serialize("negative zero", "-0", false, "-0");
    expect_serialize("exponent", "1e+28", false, "1e+28");
    expect_serialize("normalized exponent", "20e1", false, "200");
    expect_serialize("string", "\"hello\"", false, "\"hello\"");
    expect_serialize("empty string", "\"\"", false, "\"\"");
    expect_serialize("escaped string", "\"a\\nb\\\"c\\\\d\\u0041\"", false, "\"a\\nb\\\"c\\\\d\\u0041\"");
    expect_serialize("array", "[1, 2, 3]", false, "[1,2,3]");
    expect_serialize("empty array", "[ ]", false, "[]");
    expect_serialize("object", "{\"a\": 1, \"b\": [true, null]}", false, "{\"a\":1,\"b\":[true,null]}");
    expect_serialize("empty object", "{ }", false, "{}");
    expect_serialize("escaped key", "{\"a\\nb\": 1}", false, "{\"a\\nb\":1}");

    expect_serialize("pretty array", "[1,2]", true, "[\n  1,\n  2\n]");
    expect_serialize("pretty empty array", "[ ]", true, "[]");
    expect_serialize("pretty object", "{\"a\":1,\"b\":[true,null]}", true,
                     "{\n  \"a\": 1,\n  \"b\": [\n    true,\n    null\n  ]\n}");
    expect_serialize("pretty nested array", "[[1],[2]]", true,
                     "[\n  [\n    1\n  ],\n  [\n    2\n  ]\n]");
    expect_serialize("pretty empty object", "{ }", true, "{}");
    expect_serialize("pretty scalar", "42", true, "42");

    expect_roundtrip("round-trip object", "{\"a\":1,\"b\":[true,false,null,1.5]}", false);
    expect_roundtrip("round-trip escapes", "\"a\\nb\\\"c\\\\d\\u0041\"", false);
    expect_roundtrip("round-trip unicode", "\"\\u00e9\\uD834\\uDD1E\"", false);
    expect_roundtrip("round-trip big exponent", "123e65", false);
    expect_roundtrip("round-trip small exponent", "-1e-78", false);
    expect_roundtrip("round-trip pretty", "{\"a\":[1,{\"b\":2}]}", true);

    expect_parse_failure("overflow number", "1e999");
    expect_parse_failure("negative overflow number", "-1e999");
    expect_locale_numbers();

    expect_error("null value", NULL);
    simson_value_t* number = simson_value_create(SIMSON_NUMBER);
    if (!number) {
        printf("FAIL: could not create number value\n");
        return 1;
    }
    number->as.number = INFINITY;
    expect_error("infinite number", number);
    number->as.number = NAN;
    expect_error("NaN number", number);
    simson_value_destroy(number);

    if (failures > 0) {
        printf("%d failure(s)\n", failures);
        return 1;
    }
    printf("All serialization tests passed\n");
    return 0;
}
