#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simson.h"

int main(void) {
    const char* input = "{\"name\":\"simson\",\"tags\":[\"json\",\"c\"]}";
    simson_value_t* root = simson_value_parse(input, strlen(input));
    if (!root) {
        fputs("parse failed\n", stderr);
        return 1;
    }

    simson_value_t* tags = NULL;
    if (simson_object_get(root, "tags", 4, &tags) == 0) {
        if (simson_array_append(tags, simson_value_string("small", 5)) != 0) {
            fputs("append failed\n", stderr);
            simson_value_destroy(root);
            return 1;
        }
    }

    if (simson_object_set(root, "active", 6, simson_value_bool(true)) != 0) {
        fputs("set failed\n", stderr);
        simson_value_destroy(root);
        return 1;
    }

    char* output = NULL;
    size_t output_len = 0;
    if (simson_value_serialize(root, true, &output, &output_len) != 0) {
        fputs("serialize failed\n", stderr);
        simson_value_destroy(root);
        return 1;
    }

    fwrite(output, 1, output_len, stdout);
    fputc('\n', stdout);
    free(output);
    simson_value_destroy(root);
    return 0;
}
