# Test source files
set(
    test_sources
    curry_tests.cpp
    literals_tests.cpp
    overload_tests.cpp
    traits_tests.cpp
    with_arity_tests.cpp
    tests.cpp
)

# Add target to run all tests
add_executable(tests ${test_sources})
set_target_properties(tests PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(tests PUBLIC fun)
set_property(TARGET tests PROPERTY CXX_STANDARD 20)