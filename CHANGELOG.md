# Changelog

## [1.0.0] - 2026-11-01

### Added
- Structured parse errors with code, byte offset, line, column, and message.
- libFuzzer harness and CI fuzz job.
- Windows DLL export macros and hidden ELF visibility.
- pkg-config file.

### Changed
- `simson_value_parse` now takes `simson_error_t*`.
- Public ABI policy documented.

## [0.1.0] - 2026-11-01

### Added
- Initial JSON parser, serializer, CRUD API, tests, examples, install/export support.
