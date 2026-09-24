#include "test_util.h"

#include <string.h>

int g_test_failures = 0;

static int hex_nibble(char c, uint8_t *out)
{
    if (c >= '0' && c <= '9') {
        *out = (uint8_t)(c - '0');
        return 1;
    }
    if (c >= 'a' && c <= 'f') {
        *out = (uint8_t)(c - 'a' + 10);
        return 1;
    }
    if (c >= 'A' && c <= 'F') {
        *out = (uint8_t)(c - 'A' + 10);
        return 1;
    }
    return 0;
}

size_t test_hex_decode(const char *hex, uint8_t *out, size_t out_cap)
{
    size_t hex_len = strlen(hex);
    size_t byte_count;
    size_t i;

    if ((hex_len % 2U) != 0U) {
        return (size_t)-1;
    }

    byte_count = hex_len / 2U;
    if (byte_count > out_cap) {
        return (size_t)-1;
    }

    for (i = 0U; i < byte_count; i++) {
        uint8_t hi, lo;
        if (!hex_nibble(hex[2U * i], &hi) || !hex_nibble(hex[2U * i + 1U], &lo)) {
            return (size_t)-1;
        }
        out[i] = (uint8_t)((hi << 4) | lo);
    }

    return byte_count;
}

int test_hex_eq(const uint8_t *data, size_t len, const char *hex)
{
    uint8_t decoded[256];
    size_t decoded_len = test_hex_decode(hex, decoded, sizeof(decoded));

    if (decoded_len == (size_t)-1) {
        return 0;
    }
    if (decoded_len != len) {
        return 0;
    }
    return memcmp(data, decoded, len) == 0;
}
