/**
 * scale-embed: fixed-width integers, bool, and portable u128.
 *
 * All fixed-width numeric serialization is explicit little-endian,
 * independent of host endianness, and does not depend on sizeof(int),
 * sizeof(long), or sizeof(size_t).
 */
#ifndef SCALE_EMBED_PRIMITIVE_H
#define SCALE_EMBED_PRIMITIVE_H

#include <stdbool.h>
#include <stdint.h>

#include "scale_embed/error.h"
#include "scale_embed/reader.h"
#include "scale_embed/writer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- unsigned fixed-width integers ---- */

scale_result_t scale_write_u8(scale_writer_t *writer, uint8_t value);
scale_result_t scale_read_u8(scale_reader_t *reader, uint8_t *value);

scale_result_t scale_write_u16(scale_writer_t *writer, uint16_t value);
scale_result_t scale_read_u16(scale_reader_t *reader, uint16_t *value);

scale_result_t scale_write_u32(scale_writer_t *writer, uint32_t value);
scale_result_t scale_read_u32(scale_reader_t *reader, uint32_t *value);

scale_result_t scale_write_u64(scale_writer_t *writer, uint64_t value);
scale_result_t scale_read_u64(scale_reader_t *reader, uint64_t *value);

/* ---- signed fixed-width integers (two's complement, little-endian) ---- */

scale_result_t scale_write_i8(scale_writer_t *writer, int8_t value);
scale_result_t scale_read_i8(scale_reader_t *reader, int8_t *value);

scale_result_t scale_write_i16(scale_writer_t *writer, int16_t value);
scale_result_t scale_read_i16(scale_reader_t *reader, int16_t *value);

scale_result_t scale_write_i32(scale_writer_t *writer, int32_t value);
scale_result_t scale_read_i32(scale_reader_t *reader, int32_t *value);

scale_result_t scale_write_i64(scale_writer_t *writer, int64_t value);
scale_result_t scale_read_i64(scale_reader_t *reader, int64_t *value);

/* ---- bool: false = 0x00, true = 0x01; any other decoded byte is invalid ---- */

scale_result_t scale_write_bool(scale_writer_t *writer, bool value);
scale_result_t scale_read_bool(scale_reader_t *reader, bool *value);

/* ---- portable u128: no compiler __int128 required ---- */

typedef struct {
    uint64_t lo;
    uint64_t hi;
} scale_u128_t;

/** Encoded as 16 bytes little-endian: lo's 8 LE bytes, then hi's 8 LE bytes. */
scale_result_t scale_write_u128(scale_writer_t *writer, scale_u128_t value);
scale_result_t scale_read_u128(scale_reader_t *reader, scale_u128_t *value);

#ifdef __cplusplus
}
#endif

#endif /* SCALE_EMBED_PRIMITIVE_H */
