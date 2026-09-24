# scale-embed

A minimal, deterministic, embedded-friendly [SCALE codec](https://docs.substrate.io/reference/scale-codec/)
implemented in pure C, with no mandatory heap allocation, no C++ dependency, and no
exceptions. Designed for constrained targets (Zephyr, ESP-IDF, STM32, RIOT, bare-metal
C, Arduino-compatible environments) as a reusable low-level building block for
generated C runtime bindings such as `robonomics-runtime-ctypes`.

`scale-embed` is intentionally independent of Robonomics, Substrate RPC, networking,
and cryptography — it only implements SCALE serialization rules.

See https://github.com/airalab/robonomics/issues/664 for the full design rationale.

## Goals

- Pure C, compiles as C99 or C11.
- No heap allocation by default; operates on caller-provided buffers.
- Deterministic memory usage, suitable for small embedded systems.
- Stable, simple `scale_`-prefixed C API.
- Encoding and decoding with explicit `scale_result_t` error codes — never aborts,
  asserts on user input, throws, or silently truncates.
- Byte-for-byte compatible with the official Rust `parity-scale-codec` implementation.

## Non-goals

`scale-embed` does not parse runtime metadata, implement Substrate RPC, build
extrinsics, sign payloads, implement SS58, or implement cryptography. It has no
required C++/STL/exceptions/RTTI dependency and performs no internal heap
allocation.

## Build

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

CMake options:

- `SCALE_EMBED_BUILD_TESTS` (default `ON`)
- `SCALE_EMBED_BUILD_EXAMPLES` (default `ON`)
- `SCALE_EMBED_STRICT_WARNINGS` (default `ON`) — builds with
  `-Wall -Wextra -Wpedantic -Wconversion -Wshadow` on GCC/Clang.

To use from another CMake project:

```cmake
add_subdirectory(scale-embed)
target_link_libraries(app PRIVATE scale-embed)
```

## Usage

```c
#include <scale_embed/scale.h>

uint8_t buffer[256];
scale_writer_t writer;
scale_writer_init(&writer, buffer, sizeof(buffer));

scale_result_t result = scale_write_u32(&writer, 42);
if (result != SCALE_OK) {
    /* handle error */
}
```

See `examples/encode_record.c` for a larger example composing several SCALE
primitives via the `SCALE_TRY` macro.

## Supported SCALE features

- Fixed-width integers: `u8`/`u16`/`u32`/`u64`, `i8`/`i16`/`i32`/`i64` (little-endian).
- Portable `u128` (`scale_u128_t { uint64_t lo, hi; }`, no compiler `__int128` required).
- `bool`.
- `Compact<u32>` / `Compact<u64>` (all four SCALE compact modes).
- Raw bytes (`scale_write_raw` / `scale_read_raw`, no length prefix).
- SCALE byte sequences (`Vec<u8>`-style: `Compact(len)` + raw bytes), including a
  caller-buffer decoder (`scale_read_bytes`) and a zero-copy view decoder
  (`scale_read_bytes_view`).
- `Option` tag helpers (`scale_write_option_none/some`, `scale_read_option_tag`).
- `Result<T, E>` tag helpers (`scale_write_result_ok/err`, `scale_read_result_tag`).
- Variant/enum discriminant helpers (`scale_write_variant`, `scale_read_variant`).

Tuples, structs, and generic sequences are intentionally **not** modeled by
`scale-embed` — generated runtime bindings are expected to compose the primitives
above (see `examples/encode_record.c`).

## Unsupported / deferred SCALE features

Deferred as explicit follow-up work (tracked from the parent issue's "Can be
deferred" list), not silently dropped:

- `u256`.
- UTF-8 string helpers (encode like `Vec<u8>` today via `scale_write_bytes`).
- `BitSequence`.
- Generic sequence/iterator callbacks.
- Optional allocator-based convenience API.
- Fuzzing harness and sanitizer CI.
- C++ wrappers, streaming writer callbacks.
- A Rust (`parity-scale-codec`) reference fixture generator/verifier project
  (`tests/reference-rust/`), and a native Zephyr module (`zephyr/module.yml`,
  `Kconfig`). These were part of the original issue's milestone but require a
  Rust toolchain / Zephyr SDK that was not available in the environment this
  initial implementation was produced in. The C API and directory layout are
  designed so both can be added later without breaking changes.

## Buffer sizing & lifetime rules

- `scale_writer_t` and `scale_reader_t` never allocate; they operate entirely on
  a caller-provided buffer whose lifetime must outlive the writer/reader.
- A failed write leaves `writer->offset` unchanged (transactional); a failed read
  leaves `reader->offset` unchanged.
- `scale_read_bytes` requires the caller-provided `scale_bytes_t::capacity` to be
  large enough for the decoded length, returning `SCALE_ERROR_BUFFER_TOO_SMALL`
  otherwise; no partial/truncated copy is performed.
- `scale_read_bytes_view` returns a `scale_bytes_view_t` pointing **directly into
  the reader's input buffer**. The view is only valid as long as the underlying
  buffer passed to `scale_reader_init` remains valid and unmodified.

## Errors

All fallible operations return a `scale_result_t` (see
`include/scale_embed/error.h`). The `SCALE_TRY(expr)` macro (also in `error.h`,
re-exported from `scale.h`) simplifies propagating errors in generated code, but
the API remains fully usable without it.

## Testing

`tests/vectors/*.json` hold the conformance fixtures (encoded hex + expected
values, plus malformed/negative cases) that `tests/c/test_*.c` are written
against; each C test function mirrors the vectors in the matching JSON file
so the two stay easy to cross-check by eye. There is no external test
framework or JSON parser dependency: `tests/c/test_util.c` provides a small
`CHECK(...)` assertion macro and a hex encode/decode helper, and
`tests/c/test_main.c` aggregates all suites into a single executable wired
into CTest (`ctest --test-dir build`).

Tests have been verified to build and pass under both C11 and C99, and
under AddressSanitizer + UndefinedBehaviorSanitizer
(`-fsanitize=address,undefined`).

## Relationship to `parity-scale-codec` and `scale-codec-cpp`

`parity-scale-codec` (Rust) is the authoritative behavioral reference for SCALE
encoding; where any other reference disagrees, `parity-scale-codec` wins.
`scale-codec-cpp` was reviewed as an additional implementation reference for
algorithms, edge cases, and test scenarios, translated to the embedded C API and
memory model described above (see the mapping table in the issue). Conformance
vectors under `tests/vectors/` are hand-derived from the publicly documented SCALE
Compact algorithm and known example encodings; each vector file notes its
derivation so it can be cross-checked later against a live `parity-scale-codec`
run once a Rust toolchain is available (see `tests/reference-rust/` follow-up
above).
