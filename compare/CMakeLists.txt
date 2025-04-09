set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/)

find_package(OpenCL 2.0 REQUIRED)

set(KERNEL_SOURCE     ${CMAKE_SOURCE_DIR}/include/BitonicSort/cl/kernels/bitonic_sort.cl)
set(KERNEL_OUTPUT_DIR ${CMAKE_BINARY_DIR}/compare)
file(COPY ${KERNEL_SOURCE} DESTINATION ${KERNEL_OUTPUT_DIR})

add_executable(cmp_bitonic_sort bitonic_sort_cmp.cpp)
target_sources(cmp_bitonic_sort
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${INCLUDE_DIR}
)
target_link_libraries(cmp_bitonic_sort PRIVATE OpenCL::OpenCL)