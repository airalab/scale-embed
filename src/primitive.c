#include "scale_embed/primitive.h"

/* Internal helpers: encode/decode an unsigned value of `width` bytes (1, 2,
 * 4, or 8) as little-endian, carried in a uint64_t. Byte-by-byte so the
 * result never depends on host endianness. */

static scale_result_t write_uint_le(
    scale_writer_t *writer,
    uint64_t value,
    size_t width
)
{
    uint8_t bytes[8];
    size_t i;

    for (i = 0U; i < width; i++) {
        bytes[i] = (uint8_t)(value >> (8U * i));
    }

    return scale_writer_write(writer, bytes, width);
}

static scale_result_t read_uint_le(
    scale_reader_t *reader,
    uint64_t *value,
    size_t width
)
{
    uint8_t bytes[8];
    scale_result_t result;
    size_t i;
    uint64_t accumulated;

    result = scale_reader_read(reader, bytes, width);
    if (result != SCALE_OK) {
        return result;
    }

    accumulated = 0U;
    for (i = 0U; i < width; i++) {
        accumulated |= ((uint64_t)bytes[i]) << (8U * i);
    }

    *value = accumulated;
    return SCALE_OK;
}

/* ---- unsigned fixed-width integers ---- */

scale_result_t scale_write_u8(scale_writer_t *writer, uint8_t value)
{
    return write_uint_le(writer, value, 1U);
}

scale_result_t scale_read_u8(scale_reader_t *reader, uint8_t *value)
{
    uint64_t raw;
    scale_result_t result = read_uint_le(reader, &raw, 1U);
    if (result != SCALE_OK) {
        return result;
    }
    *value = (uint8_t)raw;
    return SCALE_OK;
}

scale_result_t scale_write_u16(scale_writer_t *writer, uint16_t value)
{
    return write_uint_le(writer, value, 2U);
}

scale_result_t scale_read_u16(scale_reader_t *reader, uint16_t *value)
{
    uint64_t raw;
    scale_result_t result = read_uint_le(reader, &raw, 2U);
    if (result != SCALE_OK) {
        return result;
    }
    *value = (uint16_t)raw;
    return SCALE_OK;
}

scale_result_t scale_write_u32(scale_writer_t *writer, uint32_t value)
{
    return write_uint_le(writer, value, 4U);
}

scale_result_t scale_read_u32(scale_reader_t *reader, uint32_t *value)
{
    uint64_t raw;
    scale_result_t result = read_uint_le(reader, &raw, 4U);
    if (result != SCALE_OK) {
        return result;
    }
    *value = (uint32_t)raw;
    return SCALE_OK;
}

scale_result_t scale_write_u64(scale_writer_t *writer, uint64_t value)
{
    return write_uint_le(writer, value, 8U);
}

scale_result_t scale_read_u64(scale_reader_t *reader, uint64_t *value)
{
    return read_uint_le(reader, value, 8U);
}

/* ---- signed fixed-width integers ---- */

scale_result_t scale_write_i8(scale_writer_t *writer, int8_t value)
{
    return scale_write_u8(writer, (uint8_t)value);
}

scale_result_t scale_read_i8(scale_reader_t *reader, int8_t *value)
{
    uint8_t raw;
    scale_result_t result = scale_read_u8(reader, &raw);
    if (result != SCALE_OK) {
        return result;
    }
    *value = (int8_t)raw;
    return SCALE_OK;
}

scale_result_t scale_write_i16(scale_writer_t *writer, int16_t value)
{
    return scale_write_u16(writer, (uint16_t)value);
}

scale_result_t scale_read_i16(scale_reader_t *reader, int16_t *value)
{
    uint16_t raw;
    scale_result_t result = scale_read_u16(reader, &raw);
    if (result != SCALE_OK) {
        return result;
    }
    *value = (int16_t)raw;
    return SCALE_OK;
}

scale_result_t scale_write_i32(scale_writer_t *writer, int32_t value)
{
    return scale_write_u32(writer, (uint32_t)value);
}

scale_result_t scale_read_i32(scale_reader_t *reader, int32_t *value)
{
    uint32_t raw;
    scale_result_t result = scale_read_u32(reader, &raw);
    if (result != SCALE_OK) {
        return result;
    }
    *value = (int32_t)raw;
    return SCALE_OK;
}

scale_result_t scale_write_i64(scale_writer_t *writer, int64_t value)
{
    return scale_write_u64(writer, (uint64_t)value);
}

scale_result_t scale_read_i64(scale_reader_t *reader, int64_t *value)
{
    uint64_t raw;
    scale_result_t result = scale_read_u64(reader, &raw);
    if (result != SCALE_OK) {
        return result;
    }
    *value = (int64_t)raw;
    return SCALE_OK;
}

/* ---- bool ---- */

scale_result_t scale_write_bool(scale_writer_t *writer, bool value)
{
    return scale_write_u8(writer, value ? 0x01U : 0x00U);
}

scale_result_t scale_read_bool(scale_reader_t *reader, bool *value)
{
    size_t saved_offset = reader->offset;
    uint8_t raw;
    scale_result_t result = scale_read_u8(reader, &raw);
    if (result != SCALE_OK) {
        return result;
    }

    if (raw == 0x00U) {
        *value = false;
    } else if (raw == 0x01U) {
        *value = true;
    } else {
        reader->offset = saved_offset;
        return SCALE_ERROR_INVALID_BOOL;
    }

    return SCALE_OK;
}

/* ---- portable u128 ---- */

scale_result_t scale_write_u128(scale_writer_t *writer, scale_u128_t value)
{
    /* Check capacity for both halves up front so a failure never leaves the
     * writer partially advanced. */
    if (writer == NULL) {
        return SCALE_ERROR_INVALID_ARGUMENT;
    }
    if (scale_writer_remaining(writer) < 16U) {
        return SCALE_ERROR_BUFFER_TOO_SMALL;
    }

    SCALE_TRY(scale_write_u64(writer, value.lo));
    SCALE_TRY(scale_write_u64(writer, value.hi));
    return SCALE_OK;
}

scale_result_t scale_read_u128(scale_reader_t *reader, scale_u128_t *value)
{
    scale_u128_t out;

    /* Check availability for both halves up front so a failure never
     * leaves the reader partially advanced. */
    if (reader == NULL || value == NULL) {
        return SCALE_ERROR_INVALID_ARGUMENT;
    }
    if (scale_reader_remaining(reader) < 16U) {
        return SCALE_ERROR_UNEXPECTED_EOF;
    }

    SCALE_TRY(scale_read_u64(reader, &out.lo));
    SCALE_TRY(scale_read_u64(reader, &out.hi));
    *value = out;
    return SCALE_OK;
}
