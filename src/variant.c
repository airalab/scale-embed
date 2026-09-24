#include "scale_embed/variant.h"

#include "scale_embed/primitive.h"

scale_result_t scale_write_variant(
    scale_writer_t *writer,
    uint8_t index
)
{
    return scale_write_u8(writer, index);
}

scale_result_t scale_read_variant(
    scale_reader_t *reader,
    uint8_t *index
)
{
    return scale_read_u8(reader, index);
}
