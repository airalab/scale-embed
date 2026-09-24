/**
 * scale-embed: bounds-checked reader over a caller-provided, read-only
 * buffer.
 *
 * No allocation ever occurs. A failed read leaves the reader's offset
 * unchanged (transactional behavior).
 */
#ifndef SCALE_EMBED_READER_H
#define SCALE_EMBED_READER_H

#include <stddef.h>
#include <stdint.h>

#include "scale_embed/error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const uint8_t *data;
    size_t size;
    size_t offset;
} scale_reader_t;

/**
 * Initialize `reader` to read from `data`, which must remain valid and
 * unmodified for the lifetime of the reader (and of any zero-copy views
 * produced from it). `size` is the size of `data` in bytes.
 */
void scale_reader_init(
    scale_reader_t *reader,
    const uint8_t *data,
    size_t size
);

/** Number of bytes consumed so far. */
size_t scale_reader_consumed(
    const scale_reader_t *reader
);

/** Number of bytes still available before the input is exhausted. */
size_t scale_reader_remaining(
    const scale_reader_t *reader
);

/**
 * Copy `len` raw bytes from the reader into `out`, bounds-checked.
 * On SCALE_ERROR_UNEXPECTED_EOF, the reader offset is left unchanged.
 * Internal helper shared by all decoders.
 */
scale_result_t scale_reader_read(
    scale_reader_t *reader,
    uint8_t *out,
    size_t len
);

/**
 * Obtain a pointer to `len` bytes at the current reader offset without
 * copying, bounds-checked, and advance the reader past them on success.
 * The returned pointer aliases the reader's input buffer and is valid only
 * as long as that buffer is valid and unmodified.
 */
scale_result_t scale_reader_view(
    scale_reader_t *reader,
    size_t len,
    const uint8_t **out
);

#ifdef __cplusplus
}
#endif

#endif /* SCALE_EMBED_READER_H */
