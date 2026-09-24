/**
 * scale-embed: SCALE Result<T, E> tag helpers.
 *
 * SCALE Result representation: Ok(T) = variant 0 followed by T, Err(E) =
 * variant 1 followed by E. Named to avoid clashing with the codec's own
 * `scale_result_t` operation-status type: these functions deal only with
 * the SCALE `Result<T, E>` *value* tag.
 */
#ifndef SCALE_EMBED_RESULT_H
#define SCALE_EMBED_RESULT_H

#include <stdbool.h>

#include "scale_embed/error.h"
#include "scale_embed/reader.h"
#include "scale_embed/writer.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Write the Result::Ok tag (variant 0). Caller encodes T afterward. */
scale_result_t scale_write_result_ok(
    scale_writer_t *writer
);

/** Write the Result::Err tag (variant 1). Caller encodes E afterward. */
scale_result_t scale_write_result_err(
    scale_writer_t *writer
);

/**
 * Read the Result tag and report whether it was Ok via *is_ok. Any tag
 * other than 0 or 1 is rejected with SCALE_ERROR_INVALID_VARIANT.
 */
scale_result_t scale_read_result_tag(
    scale_reader_t *reader,
    bool *is_ok
);

#ifdef __cplusplus
}
#endif

#endif /* SCALE_EMBED_RESULT_H */
