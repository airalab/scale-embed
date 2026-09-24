/**
 * Tests for SCALE enum/variant discriminant helpers. Vectors mirrored from
 * tests/vectors/variant.json.
 */
#include "test_util.h"

#include "scale_embed/scale.h"

void test_variant_run(void)
{
    uint8_t buf[4];
    scale_writer_t w;
    scale_reader_t r;
    uint8_t out;

    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_variant(&w, 0U) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), "00"));

    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_variant(&w, 7U) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), "07"));
    scale_reader_init(&r, buf, scale_writer_size(&w));
    CHECK(scale_read_variant(&r, &out) == SCALE_OK);
    CHECK(out == 7U);

    scale_writer_init(&w, buf, sizeof(buf));
    CHECK(scale_write_variant(&w, 255U) == SCALE_OK);
    CHECK(test_hex_eq(buf, scale_writer_size(&w), "ff"));
    scale_reader_init(&r, buf, scale_writer_size(&w));
    CHECK(scale_read_variant(&r, &out) == SCALE_OK);
    CHECK(out == 255U);

    /* malformed: empty input */
    {
        scale_reader_init(&r, buf, 0U);
        CHECK(scale_read_variant(&r, &out) == SCALE_ERROR_UNEXPECTED_EOF);
    }
}
