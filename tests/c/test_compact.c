/**
 * Tests for Compact<u32>/Compact<u64>/Compact<u128>. u32/u64 vectors are
 * mirrored from tests/vectors/compact.json (values there sourced from
 * parity-scale-codec's own test suite -- see the vector file's
 * "source_url"). u128 vectors were independently derived from the same
 * encoding algorithm (see the vector file's "note" for detail).
 */
#include "test_util.h"

#include "scale_embed/scale.h"

static void check_compact_u32(uint32_t value, const char *hex)
{
    uint8_t buf[16];
    scale_writer_t w;
    scale_reader_t r;
    uint32_t out;

    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_compact_u32(&w, value) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), hex));

    scale_reader_init(&r, buf, scale_writer_size(&w));
    CHECK(scale_read_compact_u32(&r, &out) == SCALE_OK);
    CHECK(out == value);
    CHECK(scale_reader_remaining(&r) == 0U);
}

static void check_compact_u64(uint64_t value, const char *hex)
{
    uint8_t buf[16];
    scale_writer_t w;
    scale_reader_t r;
    uint64_t out;

    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_compact_u64(&w, value) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), hex));

    scale_reader_init(&r, buf, scale_writer_size(&w));
    CHECK(scale_read_compact_u64(&r, &out) == SCALE_OK);
    CHECK(out == value);
    CHECK(scale_reader_remaining(&r) == 0U);
}

static void check_compact_u128(uint64_t lo, uint64_t hi, const char *hex)
{
    uint8_t buf[18];
    scale_writer_t w;
    scale_reader_t r;
    scale_u128_t value;
    scale_u128_t out;

    value.lo = lo;
    value.hi = hi;

    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_compact_u128(&w, value) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), hex));

    scale_reader_init(&r, buf, scale_writer_size(&w));
    CHECK(scale_read_compact_u128(&r, &out) == SCALE_OK);
    CHECK(out.lo == lo && out.hi == hi);
    CHECK(scale_reader_remaining(&r) == 0U);
}

void test_compact_run(void)
{
    /* compact_u32 boundary values */
    check_compact_u32(0U, "00");
    check_compact_u32(63U, "fc");
    check_compact_u32(64U, "0101");
    check_compact_u32(16383U, "fdff");
    check_compact_u32(16384U, "02000100");
    check_compact_u32(1073741823U, "feffffff");
    check_compact_u32(1073741824U, "0300000040");
    check_compact_u32(4294967295U, "03ffffffff");

    /* compact_u64 boundary values, including beyond u32 range */
    check_compact_u64(0ULL, "00");
    check_compact_u64(63ULL, "fc");
    check_compact_u64(64ULL, "0101");
    check_compact_u64(16383ULL, "fdff");
    check_compact_u64(16384ULL, "02000100");
    check_compact_u64(1073741823ULL, "feffffff");
    check_compact_u64(1073741824ULL, "0300000040");
    check_compact_u64(4294967295ULL, "03ffffffff");
    check_compact_u64(4294967296ULL, "070000000001");
    check_compact_u64(1099511627776ULL, "0b000000000001");
    check_compact_u64(281474976710656ULL, "0f00000000000001");
    check_compact_u64(72057594037927935ULL, "0fffffffffffffff");
    check_compact_u64(72057594037927936ULL, "130000000000000001");
    check_compact_u64(UINT64_MAX, "13ffffffffffffffff");

    /* compact_u128 boundary values, reusing u64 modes for small values and
     * exercising big-integer mode up to the full 16-byte width */
    check_compact_u128(0U, 0U, "00");
    check_compact_u128(63U, 0U, "fc");
    check_compact_u128(64U, 0U, "0101");
    check_compact_u128(1073741823U, 0U, "feffffff");
    check_compact_u128(1073741824U, 0U, "0300000040");
    check_compact_u128(UINT64_MAX, 0U, "13ffffffffffffffff");
    check_compact_u128(0U, 1U, "17000000000000000001");
    check_compact_u128(0U, 256U, "1b00000000000000000001");
    check_compact_u128(UINT64_MAX, 0x00FFFFFFFFFFFFFFULL, "2fffffffffffffffffffffffffffffff");
    check_compact_u128(0U, 0x0100000000000000ULL, "3300000000000000000000000000000001");
    check_compact_u128(UINT64_MAX, UINT64_MAX, "33ffffffffffffffffffffffffffffffff");

    /* malformed: empty input */
    {
        scale_reader_t r;
        uint64_t out;
        scale_reader_init(&r, NULL, 0U);
        CHECK(scale_read_compact_u64(&r, &out) == SCALE_ERROR_UNEXPECTED_EOF);
    }

    /* malformed: truncated two-byte mode (prefix present, second byte
     * missing) */
    {
        uint8_t input[1];
        scale_reader_t r;
        uint64_t out;

        CHECK(test_hex_decode("01", input, sizeof(input)) == 1U);
        scale_reader_init(&r, input, sizeof(input));
        CHECK(scale_read_compact_u64(&r, &out) == SCALE_ERROR_UNEXPECTED_EOF);
        CHECK(scale_reader_consumed(&r) == 0U);
    }

    /* malformed: non-canonical two-byte mode encoding of value 5 */
    {
        uint8_t input[2];
        scale_reader_t r;
        uint64_t out;

        CHECK(test_hex_decode("1500", input, sizeof(input)) == 2U);
        scale_reader_init(&r, input, sizeof(input));
        CHECK(scale_read_compact_u64(&r, &out) == SCALE_ERROR_INVALID_COMPACT);
        CHECK(scale_reader_consumed(&r) == 0U);
    }

    /* malformed: non-canonical big-integer mode (4 bytes, value fits in
     * four-byte mode) */
    {
        uint8_t input[5];
        scale_reader_t r;
        uint32_t out32;

        CHECK(test_hex_decode("0300000000", input, sizeof(input)) == 5U);
        scale_reader_init(&r, input, sizeof(input));
        CHECK(scale_read_compact_u32(&r, &out32) == SCALE_ERROR_INVALID_COMPACT);
        CHECK(scale_reader_consumed(&r) == 0U);
    }

    /* malformed: prefix implies a byte count beyond u64's 8-byte width */
    {
        uint8_t input[1] = { 0xFFU };
        scale_reader_t r;
        uint64_t out;

        scale_reader_init(&r, input, sizeof(input));
        CHECK(scale_read_compact_u64(&r, &out) == SCALE_ERROR_INVALID_COMPACT);
        CHECK(scale_reader_consumed(&r) == 0U);
    }

    /* Compact<u32> decode overflow: a valid Compact<u64> encoding of 2^32
     * does not fit in a u32, and must be rejected without consuming
     * input. */
    {
        uint8_t input[6];
        scale_reader_t r;
        uint32_t out32;

        CHECK(test_hex_decode("070000000001", input, sizeof(input)) == 6U);
        scale_reader_init(&r, input, sizeof(input));
        CHECK(scale_read_compact_u32(&r, &out32) == SCALE_ERROR_OVERFLOW);
        CHECK(scale_reader_consumed(&r) == 0U);
    }

    /* malformed: prefix implies a byte count beyond u128's 16-byte width */
    {
        uint8_t input[1] = { 0xFFU };
        scale_reader_t r;
        scale_u128_t out128;

        scale_reader_init(&r, input, sizeof(input));
        CHECK(scale_read_compact_u128(&r, &out128) == SCALE_ERROR_INVALID_COMPACT);
        CHECK(scale_reader_consumed(&r) == 0U);
    }

    /* malformed: non-canonical big-integer mode (9 bytes, value 1 with a
     * zero top byte, which should have used single-byte mode) */
    {
        uint8_t input[10];
        scale_reader_t r;
        scale_u128_t out128;

        CHECK(test_hex_decode("17010000000000000000", input, sizeof(input)) == 10U);
        scale_reader_init(&r, input, sizeof(input));
        CHECK(scale_read_compact_u128(&r, &out128) == SCALE_ERROR_INVALID_COMPACT);
        CHECK(scale_reader_consumed(&r) == 0U);
    }
}
