cmake_minimum_required(VERSION 3.30)
project(end_to_end)

find_package(Python3 REQUIRED)

set(PYTHON_SCRIPT_RUN "${CMAKE_SOURCE_DIR}/tests/end_to_end/run_tests.py")
add_test(
    NAME run_bitonic_sort_test_target
    COMMAND ${CMAKE_COMMAND} -E env ${Python3_EXECUTABLE} ${PYTHON_SCRIPT_RUN}
)
