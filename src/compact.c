#include "scale_embed/compact.h"

#include <stdint.h>

#define SCALE_COMPACT_MODE0_MAX ((uint64_t)0x3FU)          /* 63 */
#define SCALE_COMPACT_MODE1_MAX ((uint64_t)0x3FFFU)        /* 16383 */
#define SCALE_COMPACT_MODE2_MAX ((uint64_t)0x3FFFFFFFU)    /* 1073741823 == UINT32_MAX >> 2 */

/* Number of bytes (4..8) needed to hold `value` with no leading zero byte,
 * per parity-scale-codec's `8 - value.leading_zeros() / 8`. Only called for
 * value > SCALE_COMPACT_MODE2_MAX, so the result is always >= 4. */
static size_t compact_bytes_needed(uint64_t value)
{
    size_t bytes_needed = 4U;
    while (bytes_needed < 8U && (value >> (8U * bytes_needed)) != 0U) {
        bytes_needed++;
    }
    return bytes_needed;
}

scale_result_t scale_write_compact_u64(
    scale_writer_t *writer,
    uint64_t value
)
{
    if (value <= SCALE_COMPACT_MODE0_MAX) {
        return scale_writer_write(
            writer,
            (uint8_t[]){ (uint8_t)(value << 2) },
            1U
        );
    }

    if (value <= SCALE_COMPACT_MODE1_MAX) {
        uint16_t encoded = (uint16_t)((value << 2) | 0x01U);
        uint8_t bytes[2];
        bytes[0] = (uint8_t)(encoded & 0xFFU);
        bytes[1] = (uint8_t)(encoded >> 8);
        return scale_writer_write(writer, bytes, 2U);
    }

    if (value <= SCALE_COMPACT_MODE2_MAX) {
        uint32_t encoded = (uint32_t)((value << 2) | 0x02U);
        uint8_t bytes[4];
        size_t i;
        for (i = 0U; i < 4U; i++) {
            bytes[i] = (uint8_t)(encoded >> (8U * i));
        }
        return scale_writer_write(writer, bytes, 4U);
    }

    {
        size_t bytes_needed = compact_bytes_needed(value);
        uint8_t bytes[9];
        size_t i;

        bytes[0] = (uint8_t)(0x03U | ((bytes_needed - 4U) << 2));
        for (i = 0U; i < bytes_needed; i++) {
            bytes[1U + i] = (uint8_t)(value >> (8U * i));
        }

        return scale_writer_write(writer, bytes, 1U + bytes_needed);
    }
}

scale_result_t scale_write_compact_u32(
    scale_writer_t *writer,
    uint32_t value
)
{
    /* Every u32 value fits in at most 4 big-integer-mode bytes, so the
     * generic u64 encoder already produces the correct u32 encoding. */
    return scale_write_compact_u64(writer, (uint64_t)value);
}

static scale_result_t decode_compact_u64(
    scale_reader_t *reader,
    uint64_t *value
)
{
    size_t saved_offset;
    uint8_t prefix;
    scale_result_t result;
    unsigned mode;

    if (reader == NULL || value == NULL) {
        return SCALE_ERROR_INVALID_ARGUMENT;
    }

    saved_offset = reader->offset;

    result = scale_reader_read(reader, &prefix, 1U);
    if (result != SCALE_OK) {
        return result;
    }

    mode = (unsigned)(prefix & 0x03U);

    if (mode == 0U) {
        *value = (uint64_t)(prefix >> 2);
        return SCALE_OK;
    }

    if (mode == 1U) {
        uint8_t b1;
        uint16_t raw16;
        uint16_t x;

        result = scale_reader_read(reader, &b1, 1U);
        if (result != SCALE_OK) {
            reader->offset = saved_offset;
            return result;
        }

        raw16 = (uint16_t)((uint16_t)prefix | ((uint16_t)b1 << 8));
        x = (uint16_t)(raw16 >> 2);

        if (x > SCALE_COMPACT_MODE0_MAX && x <= SCALE_COMPACT_MODE1_MAX) {
            *value = (uint64_t)x;
            return SCALE_OK;
        }

        reader->offset = saved_offset;
        return SCALE_ERROR_INVALID_COMPACT;
    }

    if (mode == 2U) {
        uint8_t rest[3];
        uint32_t raw32;
        uint32_t x;

        result = scale_reader_read(reader, rest, 3U);
        if (result != SCALE_OK) {
            reader->offset = saved_offset;
            return result;
        }

        raw32 = (uint32_t)prefix
            | ((uint32_t)rest[0] << 8)
            | ((uint32_t)rest[1] << 16)
            | ((uint32_t)rest[2] << 24);
        x = raw32 >> 2;

        if (x > (uint32_t)SCALE_COMPACT_MODE1_MAX
            && (uint64_t)x <= SCALE_COMPACT_MODE2_MAX) {
            *value = (uint64_t)x;
            return SCALE_OK;
        }

        reader->offset = saved_offset;
        return SCALE_ERROR_INVALID_COMPACT;
    }

    /* mode == 3: big-integer mode. */
    {
        size_t count = (size_t)(prefix >> 2) + 4U;
        uint8_t bytes[8];
        uint64_t raw;
        size_t i;
        uint64_t min_threshold;

        if (count > 8U) {
            reader->offset = saved_offset;
            return SCALE_ERROR_INVALID_COMPACT;
        }

        result = scale_reader_read(reader, bytes, count);
        if (result != SCALE_OK) {
            reader->offset = saved_offset;
            return result;
        }

        raw = 0U;
        for (i = 0U; i < count; i++) {
            raw |= ((uint64_t)bytes[i]) << (8U * i);
        }

        if (count == 4U) {
            min_threshold = SCALE_COMPACT_MODE2_MAX;
        } else if (count == 8U) {
            min_threshold = UINT64_MAX >> 8;
        } else {
            /* count is 5, 6, or 7: canonical requires the top byte to be
             * non-zero, i.e. raw must not fit in (count - 1) bytes. */
            min_threshold = UINT64_MAX >> ((9U - count) * 8U);
        }

        if (raw > min_threshold) {
            *value = raw;
            return SCALE_OK;
        }

        reader->offset = saved_offset;
        return SCALE_ERROR_INVALID_COMPACT;
    }
}

scale_result_t scale_read_compact_u64(
    scale_reader_t *reader,
    uint64_t *value
)
{
    return decode_compact_u64(reader, value);
}

scale_result_t scale_read_compact_u32(
    scale_reader_t *reader,
    uint32_t *value
)
{
    size_t saved_offset;
    uint64_t raw;
    scale_result_t result;

    if (reader == NULL || value == NULL) {
        return SCALE_ERROR_INVALID_ARGUMENT;
    }

    saved_offset = reader->offset;

    result = decode_compact_u64(reader, &raw);
    if (result != SCALE_OK) {
        return result;
    }

    if (raw > (uint64_t)UINT32_MAX) {
        reader->offset = saved_offset;
        return SCALE_ERROR_OVERFLOW;
    }

    *value = (uint32_t)raw;
    return SCALE_OK;
}
