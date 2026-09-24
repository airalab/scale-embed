#include <stdio.h>

#include "test_util.h"

void test_integer_run(void);
void test_bool_run(void);
void test_compact_run(void);
void test_bytes_run(void);
void test_option_run(void);
void test_result_run(void);
void test_variant_run(void);

int main(void)
{
    test_integer_run();
    test_bool_run();
    test_compact_run();
    test_bytes_run();
    test_option_run();
    test_result_run();
    test_variant_run();

    if (g_test_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", g_test_failures);
        return 1;
    }

    printf("all scale-embed tests passed\n");
    return 0;
}
