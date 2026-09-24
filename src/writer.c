#include "scale_embed/writer.h"

#include <string.h>

void scale_writer_init(
    scale_writer_t *writer,
    uint8_t *buffer,
    size_t capacity
)
{
    writer->data = buffer;
    writer->capacity = capacity;
    writer->offset = 0;
}

size_t scale_writer_size(
    const scale_writer_t *writer
)
{
    return writer->offset;
}

size_t scale_writer_remaining(
    const scale_writer_t *writer
)
{
    /* offset never exceeds capacity, so this subtraction is always safe. */
    return writer->capacity - writer->offset;
}

uint8_t *scale_writer_data(
    scale_writer_t *writer
)
{
    return writer->data;
}

scale_result_t scale_writer_write(
    scale_writer_t *writer,
    const uint8_t *data,
    size_t len
)
{
    if (writer == NULL || (data == NULL && len != 0U)) {
        return SCALE_ERROR_INVALID_ARGUMENT;
    }

    if (len > scale_writer_remaining(writer)) {
        return SCALE_ERROR_BUFFER_TOO_SMALL;
    }

    if (len != 0U) {
        memcpy(writer->data + writer->offset, data, len);
        writer->offset += len;
    }

    return SCALE_OK;
}
