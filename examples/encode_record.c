/**
 * Example: composing scale-embed primitives to encode a runtime call,
 * mirroring how generated `robonomics-runtime-ctypes` bindings would use
 * scale-embed. This example has no dependency on Robonomics itself -- it
 * only demonstrates composing SCALE_TRY calls over the public API.
 */
#include <stdio.h>

#include "scale_embed/scale.h"

typedef struct {
    uint8_t pallet_index;
    uint8_t call_index;
    scale_bytes_view_t record;
} example_record_call_t;

static scale_result_t example_record_encode(
    scale_writer_t *writer,
    const example_record_call_t *call
)
{
    SCALE_TRY(scale_write_u8(writer, call->pallet_index));
    SCALE_TRY(scale_write_u8(writer, call->call_index));
    SCALE_TRY(scale_write_bytes(writer, call->record));
    return SCALE_OK;
}

int main(void)
{
    uint8_t buffer[64];
    scale_writer_t writer;
    const uint8_t payload[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    example_record_call_t call;
    size_t i;
    scale_result_t result;

    call.pallet_index = 42;
    call.call_index = 0;
    call.record.data = payload;
    call.record.len = sizeof(payload);

    scale_writer_init(&writer, buffer, sizeof(buffer));

    result = example_record_encode(&writer, &call);
    if (result != SCALE_OK) {
        fprintf(stderr, "encode failed: %d\n", (int)result);
        return 1;
    }

    printf("encoded %zu bytes:", scale_writer_size(&writer));
    for (i = 0U; i < scale_writer_size(&writer); i++) {
        printf(" %02x", scale_writer_data(&writer)[i]);
    }
    printf("\n");

    return 0;
}
