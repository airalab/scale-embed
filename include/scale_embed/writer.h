/**
 * scale-embed: bounds-checked writer over a caller-provided buffer.
 *
 * No allocation ever occurs. A failed write leaves the writer's offset
 * unchanged (transactional behavior).
 */
#ifndef SCALE_EMBED_WRITER_H
#define SCALE_EMBED_WRITER_H

#include <stddef.h>
#include <stdint.h>

#include "scale_embed/error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t offset;
} scale_writer_t;

/**
 * Initialize `writer` to write into `buffer`, which must remain valid for
 * the lifetime of the writer. `capacity` is the size of `buffer` in bytes.
 */
void scale_writer_init(
    scale_writer_t *writer,
    uint8_t *buffer,
    size_t capacity
);

/** Number of bytes written so far. */
size_t scale_writer_size(
    const scale_writer_t *writer
);

/** Number of bytes still available before the buffer is exhausted. */
size_t scale_writer_remaining(
    const scale_writer_t *writer
);

/** Pointer to the start of the underlying buffer. */
uint8_t *scale_writer_data(
    scale_writer_t *writer
);

/**
 * Append `len` raw bytes from `data` to the writer, bounds-checked.
 * On SCALE_ERROR_BUFFER_TOO_SMALL, the writer offset is left unchanged.
 * Internal helper shared by all encoders; also usable directly by
 * generated code that needs to append pre-serialized bytes.
 */
scale_result_t scale_writer_write(
    scale_writer_t *writer,
    const uint8_t *data,
    size_t len
);

#ifdef __cplusplus
}
#endif

#endif /* SCALE_EMBED_WRITER_H */
