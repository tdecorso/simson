# Changelog

## [Unreleased]

### Added
- Structured parse errors with code, byte offset, line, column, and message.
- libFuzzer harness and CI fuzz job.
- Windows DLL export macros and hidden ELF visibility.

### Changed
- `simson_value_parse` now takes `simson_error_t*`.

## [0.1.0] - 2026-11-01

### Added
- Initial JSON parser, serializer, CRUD API, tests, examples, install/export support.
