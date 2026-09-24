/**
 * scale-embed: explicit result/error codes.
 *
 * All fallible scale-embed operations return a scale_result_t instead of
 * aborting, asserting on user-controlled input, throwing, or silently
 * truncating data.
 */
#ifndef SCALE_EMBED_ERROR_H
#define SCALE_EMBED_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SCALE_OK = 0,

    SCALE_ERROR_INVALID_ARGUMENT,
    SCALE_ERROR_BUFFER_TOO_SMALL,
    SCALE_ERROR_UNEXPECTED_EOF,
    SCALE_ERROR_INVALID_BOOL,
    SCALE_ERROR_INVALID_COMPACT,
    SCALE_ERROR_OVERFLOW,
    SCALE_ERROR_INVALID_VARIANT,
    SCALE_ERROR_UNSUPPORTED,
} scale_result_t;

/**
 * Propagate the first non-SCALE_OK result from `expr` out of the calling
 * function. The API remains fully usable without this macro.
 */
#define SCALE_TRY(expr)                      \
    do {                                     \
        scale_result_t _scale_try_result = (expr); \
        if (_scale_try_result != SCALE_OK) { \
            return _scale_try_result;        \
        }                                    \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* SCALE_EMBED_ERROR_H */
