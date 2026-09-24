/**
 * Tests for fixed-width integers, bool, and u128.
 * Vectors mirrored from tests/vectors/integers.json and bool.json.
 */
#include "test_util.h"

#include "scale_embed/scale.h"

static void check_write_u8(uint8_t value, const char *hex)
{
    uint8_t buf[8];
    scale_writer_t w;
    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_u8(&w, value) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), hex));
}

static void check_roundtrip_u8(uint8_t value)
{
    uint8_t buf[8];
    scale_writer_t w;
    scale_reader_t r;
    uint8_t out;
    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_u8(&w, value) == SCALE_OK);
    scale_reader_init(&r, buf, scale_writer_size(&w));
    CHECK(scale_read_u8(&r, &out) == SCALE_OK);
    CHECK(out == value);
    CHECK(scale_reader_remaining(&r) == 0U);
}

void test_integer_run(void)
{
    /* u8 */
    check_write_u8(0, "00");
    check_write_u8(1, "01");
    check_write_u8(255, "ff");
    check_roundtrip_u8(0);
    check_roundtrip_u8(255);

    /* u16 */
    {
        uint8_t buf[8];
        scale_writer_t w;
        scale_reader_t r;
        uint16_t out;

        scale_writer_init(&w, buf, sizeof(buf));
        CHECK(scale_write_u16(&w, 4660U) == SCALE_OK); /* 0x1234 */
        CHECK(test_hex_eq(buf, scale_writer_size(&w), "3412"));

        scale_reader_init(&r, buf, scale_writer_size(&w));
        CHECK(scale_read_u16(&r, &out) == SCALE_OK);
        CHECK(out == 4660U);
    }

    /* u32 */
    {
        uint8_t buf[8];
        scale_writer_t w;
        scale_reader_t r;
        uint32_t out;

        scale_writer_init(&w, buf, sizeof(buf));
        CHECK(scale_write_u32(&w, 3735928559U) == SCALE_OK); /* 0xDEADBEEF */
        CHECK(test_hex_eq(buf, scale_writer_size(&w), "efbeadde"));

        scale_reader_init(&r, buf, scale_writer_size(&w));
        CHECK(scale_read_u32(&r, &out) == SCALE_OK);
        CHECK(out == 3735928559U);
    }

    /* u64 */
    {
        uint8_t buf[8];
        scale_writer_t w;
        scale_reader_t r;
        uint64_t out;

        scale_writer_init(&w, buf, sizeof(buf));
        CHECK(scale_write_u64(&w, 0x0123456789ABCDEFULL) == SCALE_OK);
        CHECK(test_hex_eq(buf, scale_writer_size(&w), "efcdab8967452301"));

        scale_reader_init(&r, buf, scale_writer_size(&w));
        CHECK(scale_read_u64(&r, &out) == SCALE_OK);
        CHECK(out == 0x0123456789ABCDEFULL);
    }

    /* signed integers */
    {
        uint8_t buf[8];
        scale_writer_t w;
        scale_reader_t r;
        int8_t i8_out;
        int32_t i32_out;
        int64_t i64_out;

        scale_writer_init(&w, buf, sizeof(buf));
        CHECK(scale_write_i8(&w, -1) == SCALE_OK);
        CHECK(test_hex_eq(buf, scale_writer_size(&w), "ff"));
        scale_reader_init(&r, buf, scale_writer_size(&w));
        CHECK(scale_read_i8(&r, &i8_out) == SCALE_OK);
        CHECK(i8_out == -1);

        scale_writer_init(&w, buf, sizeof(buf));
        CHECK(scale_write_i32(&w, INT32_MIN) == SCALE_OK);
        CHECK(test_hex_eq(buf, scale_writer_size(&w), "00000080"));
        scale_reader_init(&r, buf, scale_writer_size(&w));
        CHECK(scale_read_i32(&r, &i32_out) == SCALE_OK);
        CHECK(i32_out == INT32_MIN);

        scale_writer_init(&w, buf, sizeof(buf));
        CHECK(scale_write_i64(&w, INT64_MIN) == SCALE_OK);
        CHECK(test_hex_eq(buf, scale_writer_size(&w), "0000000000000080"));
        scale_reader_init(&r, buf, scale_writer_size(&w));
        CHECK(scale_read_i64(&r, &i64_out) == SCALE_OK);
        CHECK(i64_out == INT64_MIN);
    }

    /* u128 */
    {
        uint8_t buf[16];
        scale_writer_t w;
        scale_reader_t r;
        scale_u128_t value;
        scale_u128_t out;

        value.lo = 1U;
        value.hi = 0U;
        scale_writer_init(&w, buf, sizeof(buf));
        CHECK(scale_write_u128(&w, value) == SCALE_OK);
        CHECK(test_hex_eq(buf, scale_writer_size(&w), "01000000000000000000000000000000"));

        scale_reader_init(&r, buf, scale_writer_size(&w));
        CHECK(scale_read_u128(&r, &out) == SCALE_OK);
        CHECK(out.lo == 1U && out.hi == 0U);

        /* transactional buffer-too-small: 15-byte buffer can't hold 16 bytes */
        {
            uint8_t small_buf[15];
            scale_writer_t small_w;
            scale_writer_init(&small_w, small_buf, sizeof(small_buf));
            CHECK(scale_write_u128(&small_w, value) == SCALE_ERROR_BUFFER_TOO_SMALL);
            CHECK(scale_writer_size(&small_w) == 0U);
        }
    }

    /* transactional EOF: reading u32 from a 2-byte input must fail and not
     * consume any bytes */
    {
        uint8_t small_input[2] = { 0x01U, 0x02U };
        scale_reader_t r;
        uint32_t out;

        scale_reader_init(&r, small_input, sizeof(small_input));
        CHECK(scale_read_u32(&r, &out) == SCALE_ERROR_UNEXPECTED_EOF);
        CHECK(scale_reader_consumed(&r) == 0U);
    }

    /* transactional buffer-too-small: writing u64 into a 4-byte buffer
     * must fail and not advance the writer offset */
    {
        uint8_t small_buf[4];
        scale_writer_t w;
        scale_writer_init(&w, small_buf, sizeof(small_buf));
        CHECK(scale_write_u64(&w, 1U) == SCALE_ERROR_BUFFER_TOO_SMALL);
        CHECK(scale_writer_size(&w) == 0U);
    }
}
