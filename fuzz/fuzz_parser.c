#include <stdlib.h>

#include "simson.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    simson_value_t* value = simson_value_parse((const char*)data, size);
    if (!value) return 0;

    char* output = NULL;
    size_t output_len = 0;
    if (simson_value_serialize(value, false, &output, &output_len) != 0) {
        simson_value_destroy(value);
        return 0;
    }

    free(output);
    simson_value_destroy(value);
    return 0;
}
