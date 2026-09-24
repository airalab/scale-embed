#include "scale_embed/reader.h"

#include <string.h>

void scale_reader_init(
    scale_reader_t *reader,
    const uint8_t *data,
    size_t size
)
{
    reader->data = data;
    reader->size = size;
    reader->offset = 0;
}

size_t scale_reader_consumed(
    const scale_reader_t *reader
)
{
    return reader->offset;
}

size_t scale_reader_remaining(
    const scale_reader_t *reader
)
{
    /* offset never exceeds size, so this subtraction is always safe. */
    return reader->size - reader->offset;
}

scale_result_t scale_reader_read(
    scale_reader_t *reader,
    uint8_t *out,
    size_t len
)
{
    if (reader == NULL || (out == NULL && len != 0U)) {
        return SCALE_ERROR_INVALID_ARGUMENT;
    }

    if (len > scale_reader_remaining(reader)) {
        return SCALE_ERROR_UNEXPECTED_EOF;
    }

    if (len != 0U) {
        memcpy(out, reader->data + reader->offset, len);
        reader->offset += len;
    }

    return SCALE_OK;
}

scale_result_t scale_reader_view(
    scale_reader_t *reader,
    size_t len,
    const uint8_t **out
)
{
    if (reader == NULL || out == NULL) {
        return SCALE_ERROR_INVALID_ARGUMENT;
    }

    if (len > scale_reader_remaining(reader)) {
        return SCALE_ERROR_UNEXPECTED_EOF;
    }

    *out = reader->data + reader->offset;
    reader->offset += len;

    return SCALE_OK;
}
