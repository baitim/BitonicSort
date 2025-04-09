set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

find_package(OpenCL 2.0 REQUIRED)

set(KERNEL_SOURCE     ${CMAKE_SOURCE_DIR}/include/BitonicSort/cl/kernels/bitonic_sort.cl)
set(KERNEL_OUTPUT_DIR ${CMAKE_BINARY_DIR}/src)
file(COPY ${KERNEL_SOURCE} DESTINATION ${KERNEL_OUTPUT_DIR})

add_executable(bitonic_sort bitonic_sort.cpp)
target_sources(bitonic_sort
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${INCLUDE_DIR}
)
target_link_libraries(bitonic_sort PRIVATE OpenCL::OpenCL)

include(GNUInstallDirs)

add_library(BitonicSort bitonic_sort.cpp)
target_link_libraries(BitonicSort PRIVATE OpenCL::OpenCL)
target_include_directories(BitonicSort PUBLIC
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS BitonicSort EXPORT BitonicSortTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${INCLUDE_DIR}/BitonicSort DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT BitonicSortTargets
    FILE BitonicSortConfig.cmake
    DESTINATION lib/cmake/BitonicSort
)