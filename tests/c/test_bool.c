/**
 * Tests for SCALE bool. Vectors mirrored from tests/vectors/bool.json.
 */
#include "test_util.h"

#include "scale_embed/scale.h"

void test_bool_run(void)
{
    uint8_t buf[4];
    scale_writer_t w;
    scale_reader_t r;
    bool out;

    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_bool(&w, true) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), "01"));
    scale_reader_init(&r, buf, scale_writer_size(&w));
    CHECK(scale_read_bool(&r, &out) == SCALE_OK);
    CHECK(out == true);

    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_bool(&w, false) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), "00"));
    scale_reader_init(&r, buf, scale_writer_size(&w));
    CHECK(scale_read_bool(&r, &out) == SCALE_OK);
    CHECK(out == false);

    /* malformed: any byte other than 0x00/0x01 is rejected, transactionally */
    {
        uint8_t bad_02[1] = { 0x02U };
        uint8_t bad_ff[1] = { 0xFFU };

        scale_reader_init(&r, bad_02, 1U);
        CHECK(scale_read_bool(&r, &out) == SCALE_ERROR_INVALID_BOOL);
        CHECK(scale_reader_consumed(&r) == 0U);

        scale_reader_init(&r, bad_ff, 1U);
        CHECK(scale_read_bool(&r, &out) == SCALE_ERROR_INVALID_BOOL);
        CHECK(scale_reader_consumed(&r) == 0U);
    }

    /* malformed: empty input */
    {
        scale_reader_init(&r, buf, 0U);
        CHECK(scale_read_bool(&r, &out) == SCALE_ERROR_UNEXPECTED_EOF);
    }

    /* NULL argument validation, without consuming input */
    {
        uint8_t input[1] = { 0x01U };

        scale_reader_init(&r, input, sizeof(input));
        CHECK(scale_read_bool(&r, NULL) == SCALE_ERROR_INVALID_ARGUMENT);
        CHECK(scale_reader_consumed(&r) == 0U);

        CHECK(scale_read_bool(NULL, &out) == SCALE_ERROR_INVALID_ARGUMENT);
    }
}
