set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

find_package(GTest REQUIRED)
find_package(OpenCL 2.0 REQUIRED)

set(KERNEL_SOURCE     ${CMAKE_SOURCE_DIR}/include/BitonicSort/cl/kernels/bitonic_sort.cl)
set(KERNEL_OUTPUT_DIR ${CMAKE_BINARY_DIR}/tests/unit)
file(COPY ${KERNEL_SOURCE} DESTINATION ${KERNEL_OUTPUT_DIR})

add_executable(unit_bitonic_sort bitonic_sort_unit_test.cpp)
target_sources(unit_bitonic_sort
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${INCLUDE_DIR}
)
target_link_libraries(unit_bitonic_sort PRIVATE GTest::GTest OpenCL::OpenCL)
set_target_properties(unit_bitonic_sort PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}"
)

set(RUN_TESTS ./unit_bitonic_sort --gtest_color=yes)
add_test(
    NAME unit_bitonic_sort_target
    COMMAND ${RUN_TESTS}
)