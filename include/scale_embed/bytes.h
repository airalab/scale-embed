/**
 * scale-embed: raw byte copies and SCALE byte-sequence (Vec<u8>-style)
 * encoding/decoding.
 *
 * Decoding never allocates: `scale_read_bytes` copies into a caller-owned
 * buffer, and `scale_read_bytes_view` returns a zero-copy view directly
 * into the reader's input buffer.
 */
#ifndef SCALE_EMBED_BYTES_H
#define SCALE_EMBED_BYTES_H

#include <stddef.h>
#include <stdint.h>

#include "scale_embed/error.h"
#include "scale_embed/reader.h"
#include "scale_embed/writer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Write `len` raw bytes from `data` with no SCALE length prefix and no
 * transformation.
 */
scale_result_t scale_write_raw(
    scale_writer_t *writer,
    const uint8_t *data,
    size_t len
);

/**
 * Read `len` raw bytes into caller-provided `data`, with no SCALE length
 * prefix expected.
 */
scale_result_t scale_read_raw(
    scale_reader_t *reader,
    uint8_t *data,
    size_t len
);

/** Non-owning immutable view over a byte sequence. */
typedef struct {
    const uint8_t *data;
    size_t len;
} scale_bytes_view_t;

/**
 * Encode a SCALE Vec<u8>-style byte sequence: Compact(length) followed by
 * the raw bytes.
 */
scale_result_t scale_write_bytes(
    scale_writer_t *writer,
    scale_bytes_view_t value
);

/** Caller-owned destination for decoded variable-length bytes. No allocation
 * ever occurs; `capacity` bounds how many bytes may be copied in. */
typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t len;
} scale_bytes_t;

/**
 * Decode a SCALE Vec<u8>-style byte sequence into `value->data`, up to
 * `value->capacity` bytes. Returns SCALE_ERROR_BUFFER_TOO_SMALL (without
 * consuming input) if the decoded length exceeds `value->capacity`.
 */
scale_result_t scale_read_bytes(
    scale_reader_t *reader,
    scale_bytes_t *value
);

/**
 * Decode a SCALE Vec<u8>-style byte sequence as a zero-copy view directly
 * into the reader's input buffer. The view is valid only as long as the
 * buffer passed to scale_reader_init remains valid and unmodified.
 */
scale_result_t scale_read_bytes_view(
    scale_reader_t *reader,
    scale_bytes_view_t *value
);

#ifdef __cplusplus
}
#endif

#endif /* SCALE_EMBED_BYTES_H */
