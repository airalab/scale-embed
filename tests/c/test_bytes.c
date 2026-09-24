/**
 * Tests for raw bytes and SCALE Vec<u8>-style byte sequences. Vectors
 * mirrored from tests/vectors/bytes.json.
 */
#include "test_util.h"

#include "scale_embed/scale.h"

void test_bytes_run(void)
{
    /* raw bytes: no length prefix */
    {
        uint8_t buf[4];
        scale_writer_t w;
        scale_reader_t r;
        uint8_t out[3];
        const uint8_t payload[3] = { 0x01U, 0x02U, 0x03U };

        scale_writer_init(&w, buf, sizeof(buf));
        CHECK(scale_write_raw(&w, payload, sizeof(payload)) == SCALE_OK);
        CHECK(test_hex_eq(buf, scale_writer_size(&w), "010203"));

        scale_reader_init(&r, buf, scale_writer_size(&w));
        CHECK(scale_read_raw(&r, out, sizeof(out)) == SCALE_OK);
        CHECK(out[0] == 0x01U && out[1] == 0x02U && out[2] == 0x03U);
    }

    /* Vec<u8>-style encoding: empty sequence */
    {
        uint8_t buf[8];
        scale_writer_t w;
        scale_bytes_view_t view;

        view.data = NULL;
        view.len = 0U;

        scale_writer_init(&w, buf, sizeof(buf));
        CHECK(scale_write_bytes(&w, view) == SCALE_OK);
        CHECK(test_hex_eq(buf, scale_writer_size(&w), "00"));
    }

    /* Vec<u8>-style encoding: 3 bytes -> Compact(3) + data */
    {
        uint8_t buf[8];
        scale_writer_t w;
        scale_reader_t r;
        scale_bytes_view_t view;
        const uint8_t payload[3] = { 0x01U, 0x02U, 0x03U };
        uint8_t decoded[8];
        scale_bytes_t out;

        view.data = payload;
        view.len = sizeof(payload);

        scale_writer_init(&w, buf, sizeof(buf));
        CHECK(scale_write_bytes(&w, view) == SCALE_OK);
        CHECK(test_hex_eq(buf, scale_writer_size(&w), "0c010203"));

        out.data = decoded;
        out.capacity = sizeof(decoded);
        out.len = 0U;

        scale_reader_init(&r, buf, scale_writer_size(&w));
        CHECK(scale_read_bytes(&r, &out) == SCALE_OK);
        CHECK(out.len == 3U);
        CHECK(out.data[0] == 0x01U && out.data[1] == 0x02U && out.data[2] == 0x03U);
        CHECK(scale_reader_remaining(&r) == 0U);
    }

    /* zero-copy view decoding */
    {
        uint8_t buf[8];
        scale_writer_t w;
        scale_reader_t r;
        scale_bytes_view_t in_view;
        scale_bytes_view_t out_view;
        const uint8_t payload[3] = { 0xAAU, 0xBBU, 0xCCU };

        in_view.data = payload;
        in_view.len = sizeof(payload);

        scale_writer_init(&w, buf, sizeof(buf));
        CHECK(scale_write_bytes(&w, in_view) == SCALE_OK);

        scale_reader_init(&r, buf, scale_writer_size(&w));
        CHECK(scale_read_bytes_view(&r, &out_view) == SCALE_OK);
        CHECK(out_view.len == 3U);
        CHECK(out_view.data[0] == 0xAAU && out_view.data[1] == 0xBBU && out_view.data[2] == 0xCCU);
        /* the view aliases the reader's input buffer directly */
        CHECK(out_view.data == buf + 1);
    }

    /* malformed: truncated data (claims length 3, only 2 bytes follow) */
    {
        uint8_t input[3];
        scale_reader_t r;
        uint8_t decoded[8];
        scale_bytes_t out;

        CHECK(test_hex_decode("0c0102", input, sizeof(input)) == 3U);
        out.data = decoded;
        out.capacity = sizeof(decoded);
        out.len = 0U;

        scale_reader_init(&r, input, sizeof(input));
        CHECK(scale_read_bytes(&r, &out) == SCALE_ERROR_UNEXPECTED_EOF);
        CHECK(scale_reader_consumed(&r) == 0U);
    }

    /* malformed: destination capacity smaller than decoded length */
    {
        uint8_t input[4];
        scale_reader_t r;
        uint8_t decoded[2];
        scale_bytes_t out;

        CHECK(test_hex_decode("0c010203", input, sizeof(input)) == 4U);
        out.data = decoded;
        out.capacity = sizeof(decoded); /* 2, but decoded length is 3 */
        out.len = 0U;

        scale_reader_init(&r, input, sizeof(input));
        CHECK(scale_read_bytes(&r, &out) == SCALE_ERROR_BUFFER_TOO_SMALL);
        CHECK(scale_reader_consumed(&r) == 0U);
    }

    /* malformed: empty input */
    {
        scale_reader_t r;
        uint8_t decoded[8];
        scale_bytes_t out;

        out.data = decoded;
        out.capacity = sizeof(decoded);
        out.len = 0U;

        scale_reader_init(&r, decoded, 0U);
        CHECK(scale_read_bytes(&r, &out) == SCALE_ERROR_UNEXPECTED_EOF);
    }
}
