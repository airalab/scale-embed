/**
 * scale-embed: Compact<u32>, Compact<u64>, and Compact<u128> integer codec.
 *
 * Implements all four SCALE compact modes (single-byte, two-byte,
 * four-byte, big-integer), matching parity-scale-codec byte-for-byte,
 * including rejection of malformed and non-canonical encodings.
 */
#ifndef SCALE_EMBED_COMPACT_H
#define SCALE_EMBED_COMPACT_H

#include <stdint.h>

#include "scale_embed/error.h"
#include "scale_embed/primitive.h"
#include "scale_embed/reader.h"
#include "scale_embed/writer.h"

#ifdef __cplusplus
extern "C" {
#endif

scale_result_t scale_write_compact_u32(
    scale_writer_t *writer,
    uint32_t value
);

scale_result_t scale_read_compact_u32(
    scale_reader_t *reader,
    uint32_t *value
);

scale_result_t scale_write_compact_u64(
    scale_writer_t *writer,
    uint64_t value
);

scale_result_t scale_read_compact_u64(
    scale_reader_t *reader,
    uint64_t *value
);

scale_result_t scale_write_compact_u128(
    scale_writer_t *writer,
    scale_u128_t value
);

scale_result_t scale_read_compact_u128(
    scale_reader_t *reader,
    scale_u128_t *value
);

#ifdef __cplusplus
}
#endif

#endif /* SCALE_EMBED_COMPACT_H */
