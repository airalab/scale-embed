/**
 * Minimal test utilities shared by tests/c/test_*.c. No external test
 * framework: plain C assertions with a global failure counter, plus small
 * hex helpers so encoded-byte expectations can be written/compared as hex
 * strings mirroring tests/vectors.
 */
#ifndef SCALE_EMBED_TEST_UTIL_H
#define SCALE_EMBED_TEST_UTIL_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

extern int g_test_failures;

#define CHECK(cond)                                                       \
    do {                                                                  \
        if (!(cond)) {                                                   \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            g_test_failures++;                                           \
        }                                                                 \
    } while (0)

/* Decode a hex string (e.g. "0dead") into `out`, up to `out_cap` bytes.
 * Returns the number of bytes decoded, or (size_t)-1 on malformed input
 * (odd length or non-hex character) or if it would exceed out_cap. */
size_t test_hex_decode(const char *hex, uint8_t *out, size_t out_cap);

/* Compare `len` bytes at `data` against the bytes represented by `hex`.
 * Returns 1 if they match (and hex decodes to exactly `len` bytes), 0
 * otherwise. */
int test_hex_eq(const uint8_t *data, size_t len, const char *hex);

#endif /* SCALE_EMBED_TEST_UTIL_H */
