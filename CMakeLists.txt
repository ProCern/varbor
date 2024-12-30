cmake_minimum_required(VERSION 3.14)

project(varbor VERSION "1.0.0" DESCRIPTION "variant cbor" LANGUAGES CXX)
include(CTest)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build testing suite" OFF)

add_library(
    varbor
    INTERFACE
    )

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

install(FILES
    src/varbor.hxx
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )

install(TARGETS varbor EXPORT varborTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(
    EXPORT varborTargets
    NAMESPACE varbor::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/varbor"
)


configure_package_config_file(
    "varbor-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/varbor-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/varbor"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/varbor-config-version.cmake"
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/varbor-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/varbor-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/varbor"
)

if(BUILD_TESTING)
    add_executable(decoding_array test/decoding_array.cxx)
    target_link_libraries(decoding_array PRIVATE varbor)
    target_include_directories(decoding_array PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME decoding_array COMMAND decoding_array)

    add_executable(decoding_byte_string test/decoding_byte_string.cxx)
    target_link_libraries(decoding_byte_string PRIVATE varbor)
    target_include_directories(decoding_byte_string PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME decoding_byte_string COMMAND decoding_byte_string)

    add_executable(decoding_floats test/decoding_floats.cxx)
    target_link_libraries(decoding_floats PRIVATE varbor)
    target_include_directories(decoding_floats PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME decoding_floats COMMAND decoding_floats)

    add_executable(decoding_map test/decoding_map.cxx)
    target_link_libraries(decoding_map PRIVATE varbor)
    target_include_directories(decoding_map PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME decoding_map COMMAND decoding_map)

    add_executable(decoding_map_array_mixed_recursive test/decoding_map_array_mixed_recursive.cxx)
    target_link_libraries(decoding_map_array_mixed_recursive PRIVATE varbor)
    target_include_directories(decoding_map_array_mixed_recursive PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME decoding_map_array_mixed_recursive COMMAND decoding_map_array_mixed_recursive)

    add_executable(decoding_negative_integer test/decoding_negative_integer.cxx)
    target_link_libraries(decoding_negative_integer PRIVATE varbor)
    target_include_directories(decoding_negative_integer PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME decoding_negative_integer COMMAND decoding_negative_integer)

    add_executable(decoding_positive_integer test/decoding_positive_integer.cxx)
    target_link_libraries(decoding_positive_integer PRIVATE varbor)
    target_include_directories(decoding_positive_integer PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME decoding_positive_integer COMMAND decoding_positive_integer)

    add_executable(decoding_semantic_tag test/decoding_semantic_tag.cxx)
    target_link_libraries(decoding_semantic_tag PRIVATE varbor)
    target_include_directories(decoding_semantic_tag PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME decoding_semantic_tag COMMAND decoding_semantic_tag)

    add_executable(decoding_specials test/decoding_specials.cxx)
    target_link_libraries(decoding_specials PRIVATE varbor)
    target_include_directories(decoding_specials PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME decoding_specials COMMAND decoding_specials)

    add_executable(decoding_string test/decoding_string.cxx)
    target_link_libraries(decoding_string PRIVATE varbor)
    target_include_directories(decoding_string PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME decoding_string COMMAND decoding_string)

    add_executable(encoding_array test/encoding_array.cxx)
    target_link_libraries(encoding_array PRIVATE varbor)
    target_include_directories(encoding_array PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME encoding_array COMMAND encoding_array)

    add_executable(encoding_byte_string test/encoding_byte_string.cxx)
    target_link_libraries(encoding_byte_string PRIVATE varbor)
    target_include_directories(encoding_byte_string PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME encoding_byte_string COMMAND encoding_byte_string)

    add_executable(encoding_floats test/encoding_floats.cxx)
    target_link_libraries(encoding_floats PRIVATE varbor)
    target_include_directories(encoding_floats PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME encoding_floats COMMAND encoding_floats)

    add_executable(encoding_map test/encoding_map.cxx)
    target_link_libraries(encoding_map PRIVATE varbor)
    target_include_directories(encoding_map PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME encoding_map COMMAND encoding_map)

    add_executable(encoding_map_array_mixed_recursive test/encoding_map_array_mixed_recursive.cxx)
    target_link_libraries(encoding_map_array_mixed_recursive PRIVATE varbor)
    target_include_directories(encoding_map_array_mixed_recursive PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME encoding_map_array_mixed_recursive COMMAND encoding_map_array_mixed_recursive)

    add_executable(encoding_negative_integer test/encoding_negative_integer.cxx)
    target_link_libraries(encoding_negative_integer PRIVATE varbor)
    target_include_directories(encoding_negative_integer PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME encoding_negative_integer COMMAND encoding_negative_integer)

    add_executable(encoding_positive_integer test/encoding_positive_integer.cxx)
    target_link_libraries(encoding_positive_integer PRIVATE varbor)
    target_include_directories(encoding_positive_integer PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME encoding_positive_integer COMMAND encoding_positive_integer)

    add_executable(encoding_semantic_tag test/encoding_semantic_tag.cxx)
    target_link_libraries(encoding_semantic_tag PRIVATE varbor)
    target_include_directories(encoding_semantic_tag PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME encoding_semantic_tag COMMAND encoding_semantic_tag)

    add_executable(encoding_specials test/encoding_specials.cxx)
    target_link_libraries(encoding_specials PRIVATE varbor)
    target_include_directories(encoding_specials PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME encoding_specials COMMAND encoding_specials)

    add_executable(encoding_string test/encoding_string.cxx)
    target_link_libraries(encoding_string PRIVATE varbor)
    target_include_directories(encoding_string PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME encoding_string COMMAND encoding_string)

    add_executable(equality_array test/equality_array.cxx)
    target_link_libraries(equality_array PRIVATE varbor)
    target_include_directories(equality_array PRIVATE "${varbor_SOURCE_DIR}/src")
    add_test(NAME equality_array COMMAND equality_array)
endif()
