add_executable(scale-embed-tests
    c/test_util.c
    c/test_integer.c
    c/test_bool.c
    c/test_compact.c
    c/test_bytes.c
    c/test_option.c
    c/test_result.c
    c/test_variant.c
    c/test_main.c
)

target_include_directories(scale-embed-tests PRIVATE c)
target_link_libraries(scale-embed-tests PRIVATE scale-embed)

add_test(NAME scale-embed-tests COMMAND scale-embed-tests)
