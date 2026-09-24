/**
 * scale-embed: SCALE enum/variant discriminant helpers.
 *
 * SCALE enum/variant discriminants use a single byte. Generated runtime
 * code remains responsible for validating which indexes are valid for a
 * given enum type; scale-embed only reads/writes the raw discriminant.
 */
#ifndef SCALE_EMBED_VARIANT_H
#define SCALE_EMBED_VARIANT_H

#include <stdint.h>

#include "scale_embed/error.h"
#include "scale_embed/reader.h"
#include "scale_embed/writer.h"

#ifdef __cplusplus
extern "C" {
#endif

scale_result_t scale_write_variant(
    scale_writer_t *writer,
    uint8_t index
);

scale_result_t scale_read_variant(
    scale_reader_t *reader,
    uint8_t *index
);

#ifdef __cplusplus
}
#endif

#endif /* SCALE_EMBED_VARIANT_H */
