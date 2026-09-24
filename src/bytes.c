#include "scale_embed/bytes.h"

#include "scale_embed/compact.h"

scale_result_t scale_write_raw(
    scale_writer_t *writer,
    const uint8_t *data,
    size_t len
)
{
    return scale_writer_write(writer, data, len);
}

scale_result_t scale_read_raw(
    scale_reader_t *reader,
    uint8_t *data,
    size_t len
)
{
    return scale_reader_read(reader, data, len);
}

scale_result_t scale_write_bytes(
    scale_writer_t *writer,
    scale_bytes_view_t value
)
{
    SCALE_TRY(scale_write_compact_u32(writer, (uint32_t)value.len));
    SCALE_TRY(scale_write_raw(writer, value.data, value.len));
    return SCALE_OK;
}

scale_result_t scale_read_bytes(
    scale_reader_t *reader,
    scale_bytes_t *value
)
{
    size_t saved_offset;
    uint32_t len;
    scale_result_t result;

    if (reader == NULL || value == NULL) {
        return SCALE_ERROR_INVALID_ARGUMENT;
    }

    saved_offset = reader->offset;

    result = scale_read_compact_u32(reader, &len);
    if (result != SCALE_OK) {
        return result;
    }

    if ((size_t)len > value->capacity) {
        reader->offset = saved_offset;
        return SCALE_ERROR_BUFFER_TOO_SMALL;
    }

    result = scale_read_raw(reader, value->data, (size_t)len);
    if (result != SCALE_OK) {
        reader->offset = saved_offset;
        return result;
    }

    value->len = (size_t)len;
    return SCALE_OK;
}

scale_result_t scale_read_bytes_view(
    scale_reader_t *reader,
    scale_bytes_view_t *value
)
{
    size_t saved_offset;
    uint32_t len;
    scale_result_t result;
    const uint8_t *view;

    if (reader == NULL || value == NULL) {
        return SCALE_ERROR_INVALID_ARGUMENT;
    }

    saved_offset = reader->offset;

    result = scale_read_compact_u32(reader, &len);
    if (result != SCALE_OK) {
        return result;
    }

    result = scale_reader_view(reader, (size_t)len, &view);
    if (result != SCALE_OK) {
        reader->offset = saved_offset;
        return result;
    }

    value->data = view;
    value->len = (size_t)len;
    return SCALE_OK;
}
