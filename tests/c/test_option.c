/**
 * Tests for SCALE Option<T> tag helpers. Vectors mirrored from
 * tests/vectors/option.json.
 */
#include "test_util.h"

#include "scale_embed/scale.h"

void test_option_run(void)
{
    uint8_t buf[4];
    scale_writer_t w;
    scale_reader_t r;
    bool has_value;

    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_option_none(&w) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), "00"));
    scale_reader_init(&r, buf, scale_writer_size(&w));
    CHECK(scale_read_option_tag(&r, &has_value) == SCALE_OK);
    CHECK(has_value == false);

    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_option_some(&w) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), "01"));
    scale_reader_init(&r, buf, scale_writer_size(&w));
    CHECK(scale_read_option_tag(&r, &has_value) == SCALE_OK);
    CHECK(has_value == true);

    /* malformed: invalid tag, transactional */
    {
        uint8_t bad[1] = { 0x02U };
        scale_reader_init(&r, bad, 1U);
        CHECK(scale_read_option_tag(&r, &has_value) == SCALE_ERROR_INVALID_VARIANT);
        CHECK(scale_reader_consumed(&r) == 0U);
    }

    /* malformed: empty input */
    {
        scale_reader_init(&r, buf, 0U);
        CHECK(scale_read_option_tag(&r, &has_value) == SCALE_ERROR_UNEXPECTED_EOF);
    }
}
