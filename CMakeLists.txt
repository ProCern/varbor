cmake_minimum_required(VERSION 3.14)

project(varbor VERSION "1.0.0" DESCRIPTION "variant cbor" LANGUAGES CXX)

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
    varbor.hxx
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )

install(TARGETS varbor EXPORT varborTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT varborTargets NAMESPACE varbor:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/varbor")


configure_package_config_file("varborConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/varborConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/varbor"
    )

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/varborConfigVersion.cmake"
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/varborConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/varborConfigVersion.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/varbor"
        )

if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
        )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(
        varbor_test
        varbor_test.cxx
        )
    target_link_libraries(
        varbor_test
        GTest::gtest_main
        varbor
        )

    include(GoogleTest)
    gtest_discover_tests(varbor_test)
endif()
