#include "scale_embed/result.h"

#include "scale_embed/primitive.h"

scale_result_t scale_write_result_ok(
    scale_writer_t *writer
)
{
    return scale_write_u8(writer, 0x00U);
}

scale_result_t scale_write_result_err(
    scale_writer_t *writer
)
{
    return scale_write_u8(writer, 0x01U);
}

scale_result_t scale_read_result_tag(
    scale_reader_t *reader,
    bool *is_ok
)
{
    size_t saved_offset;
    uint8_t tag;
    scale_result_t result;

    if (reader == NULL || is_ok == NULL) {
        return SCALE_ERROR_INVALID_ARGUMENT;
    }

    saved_offset = reader->offset;

    result = scale_read_u8(reader, &tag);
    if (result != SCALE_OK) {
        return result;
    }

    if (tag == 0x00U) {
        *is_ok = true;
    } else if (tag == 0x01U) {
        *is_ok = false;
    } else {
        reader->offset = saved_offset;
        return SCALE_ERROR_INVALID_VARIANT;
    }

    return SCALE_OK;
}
