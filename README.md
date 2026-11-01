# simson

[![CI](https://github.com/tdecorso/simson/actions/workflows/ci.yml/badge.svg)](https://github.com/tdecorso/simson/actions/workflows/ci.yml)

A small JSON parser and serializer for C99.

`simson` provides parsing, serialization, and direct CRUD operations on JSON
values with explicit ownership and no external dependencies.

## Features

- Parse JSON from a length-delimited buffer (no NUL terminator required).
- Serialize compact or pretty-printed JSON.
- Typed constructors and scalar accessors.
- Array and object access and mutation.
- Insertion-ordered objects with unique keys through the mutation API.
- Round-trip-safe number formatting.
- Locale-independent number parsing and serialization.
- C++-friendly header with `extern "C"` guards.

## Build and test

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

This also builds `simson_example` from `examples/basic.c`. Run it with:

```sh
./build/simson_example
```

To build only the library without the example, configure with
`-DSIMSON_BUILD_EXAMPLES=OFF`.

To build a shared library instead of a static one:

```sh
cmake -S . -B build -DBUILD_SHARED_LIBS=ON
```

## Install and consume

```sh
cmake --install build --prefix /usr/local
```

With CMake:

```cmake
find_package(simson 0.1 CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE simson::simson)
```

A committed consumer smoke test lives in `tests/consumer`; CI uses it to verify
the installed package:

```sh
cmake --install build --prefix "$PWD/install"
cmake -S tests/consumer -B consumer-build -DCMAKE_PREFIX_PATH="$PWD/install"
cmake --build consumer-build
./consumer-build/simson_consumer
```

## Usage

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simson.h"

int main(void) {
    const char* input = "{\"name\":\"simson\",\"tags\":[\"json\",\"c\"]}";

    simson_error_t error;
    simson_value_t* root = simson_value_parse(input, strlen(input), &error);
    if (!root) return 1;

    simson_value_t* tags = NULL;
    if (simson_object_get(root, "tags", 4, &tags) == 0) {
        simson_array_append(tags, simson_value_string("small", 5));
    }
    simson_object_set(root, "active", 6, simson_value_bool(true));

    char* output = NULL;
    size_t output_len = 0;
    if (simson_value_serialize(root, true, &output, &output_len) == 0) {
        fwrite(output, 1, output_len, stdout);
        fputc('\n', stdout);
        free(output);
    }

    simson_value_destroy(root);
    return 0;
}
```

## Ownership

- Constructors, `simson_value_parse`, and `simson_*_remove` return values owned
  by the caller.
- `simson_array_append`, `simson_array_insert`, `simson_array_set`, and
  `simson_object_set` take ownership of the inserted value on success. On
  failure, the caller keeps ownership.
- Removed values are detached and returned; the caller must destroy them.
- `simson_value_destroy` recursively frees a value and its children.

## Strings

String values store raw JSON text **without** the surrounding quotes. Escape
sequences are preserved verbatim. When constructing a string, provide valid,
already-escaped JSON string content:

```c
/* Represents the JSON string "a\nb" (including the escape). */
simson_value_t* value = simson_value_string("a\\nb", 4);
```

The serializer wraps the stored text in quotes, so it does not re-escape
content. Invalid raw text will produce invalid JSON.

## Limitations

- Numbers are stored as `double`. Parsing rejects numbers that are not
  representable as finite doubles.
- Parsing accepts duplicate object keys because the input may contain them.
  The mutation API treats keys as unique and replaces the first match.
- Parse failures return `NULL` and populate `simson_error_t` with an error code,
  byte offset, line, column, and message. Serialize failures return a non-zero
  code and do not populate an error.
- Object keys are compared as raw JSON text, so `"a"` and `"\u0061"` are
  distinct keys.

## Version

The current version is `0.1.0`.

## License

MIT. See [LICENSE](LICENSE).
