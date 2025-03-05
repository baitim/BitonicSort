cmake_minimum_required(VERSION 3.11)
project(unit)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

find_package(GTest REQUIRED)
find_package(OpenCL REQUIRED)

add_executable(unit_bitonic_sort bitonic_sort_unit_test.cpp)
target_sources(unit_bitonic_sort
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${INCLUDE_DIR}
)
target_link_libraries(unit_bitonic_sort PRIVATE GTest::GTest OpenCL::OpenCL)

set(RUN_TESTS ./unit_bitonic_sort --gtest_color=yes)
add_test(
    NAME unit_bitonic_sort_target
    COMMAND ${RUN_TESTS}
)