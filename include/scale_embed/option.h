/**
 * scale-embed: SCALE Option<T> tag helpers.
 *
 * SCALE Option representation: None = variant 0, Some(T) = variant 1
 * followed by T. `scale-embed` only handles the tag byte; the caller (or
 * generated code) remains responsible for encoding/decoding T.
 */
#ifndef SCALE_EMBED_OPTION_H
#define SCALE_EMBED_OPTION_H

#include <stdbool.h>

#include "scale_embed/error.h"
#include "scale_embed/reader.h"
#include "scale_embed/writer.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Write the Option::None tag (variant 0). */
scale_result_t scale_write_option_none(
    scale_writer_t *writer
);

/** Write the Option::Some tag (variant 1). Caller encodes T afterward. */
scale_result_t scale_write_option_some(
    scale_writer_t *writer
);

/**
 * Read the Option tag and report whether a value follows via *has_value.
 * Any tag other than 0 or 1 is rejected with SCALE_ERROR_INVALID_VARIANT.
 */
scale_result_t scale_read_option_tag(
    scale_reader_t *reader,
    bool *has_value
);

#ifdef __cplusplus
}
#endif

#endif /* SCALE_EMBED_OPTION_H */
