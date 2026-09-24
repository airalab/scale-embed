/**
 * Tests for SCALE Result<T, E> tag helpers. Vectors mirrored from
 * tests/vectors/result.json.
 */
#include "test_util.h"

#include "scale_embed/scale.h"

void test_result_run(void)
{
    uint8_t buf[4];
    scale_writer_t w;
    scale_reader_t r;
    bool is_ok;

    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_result_ok(&w) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), "00"));
    scale_reader_init(&r, buf, scale_writer_size(&w));
    CHECK(scale_read_result_tag(&r, &is_ok) == SCALE_OK);
    CHECK(is_ok == true);

    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_result_err(&w) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), "01"));
    scale_reader_init(&r, buf, scale_writer_size(&w));
    CHECK(scale_read_result_tag(&r, &is_ok) == SCALE_OK);
    CHECK(is_ok == false);

    /* malformed: invalid tag, transactional */
    {
        uint8_t bad[1] = { 0x05U };
        scale_reader_init(&r, bad, 1U);
        CHECK(scale_read_result_tag(&r, &is_ok) == SCALE_ERROR_INVALID_VARIANT);
        CHECK(scale_reader_consumed(&r) == 0U);
    }

    /* malformed: empty input */
    {
        scale_reader_init(&r, buf, 0U);
        CHECK(scale_read_result_tag(&r, &is_ok) == SCALE_ERROR_UNEXPECTED_EOF);
    }
}
