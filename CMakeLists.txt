cmake_minimum_required(VERSION 3.26)

project(
    ITURBO
    VERSION 0.1.0
    DESCRIPTION "Modern C++ turbomachinery CFD solver"
    LANGUAGES CXX
)

# ------------------------------------------------------------------------------
# Global options
# ------------------------------------------------------------------------------

option(ITURBO_ENABLE_TESTS           "Build unit/integration tests" ON)
option(ITURBO_ENABLE_BENCHMARKS      "Build benchmarks" OFF)
option(ITURBO_ENABLE_APPS            "Build command-line applications" ON)
option(ITURBO_ENABLE_DOCS            "Build documentation targets" OFF)

option(TURBOCFD_ENABLE_MPI             "Enable MPI support" ON)
option(TURBOCFD_ENABLE_OPENMP          "Enable OpenMP support" ON)
option(TURBOCFD_ENABLE_PETSC           "Enable PETSc backend" OFF)
option(TURBOCFD_ENABLE_CGNS            "Enable CGNS I/O" OFF)

option(TURBOCFD_ENABLE_WARNINGS        "Enable compiler warnings" ON)
option(TURBOCFD_ENABLE_WERROR          "Treat warnings as errors" OFF)
option(TURBOCFD_ENABLE_SANITIZERS      "Enable sanitizers (non-MSVC, Debug-like builds)" OFF)
option(TURBOCFD_ENABLE_LTO             "Enable interprocedural optimization / LTO" OFF)

option(TURBOCFD_BUILD_SHARED_LIBS      "Build shared libraries" OFF)

# Standard CMake variable respected by add_library unless otherwise overridden
set(BUILD_SHARED_LIBS ${TURBOCFD_BUILD_SHARED_LIBS})

# ------------------------------------------------------------------------------
# C++ standard
# ------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# Prevent in-source builds
# ------------------------------------------------------------------------------

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed.\n"
        "Please configure in a separate build directory, e.g.\n"
        "  cmake -S . -B build\n"
    )
endif()

# ------------------------------------------------------------------------------
# Default build type for single-config generators
# ------------------------------------------------------------------------------

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        RelWithDebInfo
        CACHE STRING "Build type" FORCE
    )
endif()

# ------------------------------------------------------------------------------
# Output directories
# ------------------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ------------------------------------------------------------------------------
# Module path
# ------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# ------------------------------------------------------------------------------
# Include helper modules if present
# ------------------------------------------------------------------------------

include(CheckIPOSupported)

if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/CompilerWarnings.cmake")
    include(CompilerWarnings)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Sanitizers.cmake")
    include(Sanitizers)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/ProjectOptions.cmake")
    include(ProjectOptions)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Dependencies.cmake")
    include(Dependencies)
endif()

# ------------------------------------------------------------------------------
# IPO / LTO
# ------------------------------------------------------------------------------

if(TURBOCFD_ENABLE_LTO)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
    if(ipo_supported)
        message(STATUS "IPO/LTO enabled")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    else()
        message(WARNING "IPO/LTO not supported: ${ipo_error}")
    endif()
endif()

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

find_package(Threads REQUIRED)

if(TURBOCFD_ENABLE_OPENMP)
    find_package(OpenMP QUIET)
    if(OpenMP_CXX_FOUND)
        message(STATUS "OpenMP found")
    else()
        message(WARNING "OpenMP requested but not found")
    endif()
endif()

if(TURBOCFD_ENABLE_MPI)
    find_package(MPI QUIET COMPONENTS CXX)
    if(MPI_CXX_FOUND)
        message(STATUS "MPI found")
    else()
        message(WARNING "MPI requested but not found")
    endif()
endif()

if(TURBOCFD_ENABLE_PETSC)
    find_package(PETSc REQUIRED)
endif()

if(TURBOCFD_ENABLE_CGNS)
    find_package(CGNS REQUIRED)
endif()

# ------------------------------------------------------------------------------
# Interface target for common project settings
# ------------------------------------------------------------------------------

add_library(turbocfd_project_options INTERFACE)
add_library(TurboCFD::ProjectOptions ALIAS turbocfd_project_options)

target_compile_features(turbocfd_project_options INTERFACE cxx_std_23)

target_include_directories(turbocfd_project_options
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(turbocfd_project_options
    INTERFACE
        Threads::Threads
)

if(TURBOCFD_ENABLE_OPENMP AND OpenMP_CXX_FOUND)
    target_link_libraries(turbocfd_project_options
        INTERFACE
            OpenMP::OpenMP_CXX
    )
endif()

if(TURBOCFD_ENABLE_MPI AND MPI_CXX_FOUND)
    target_link_libraries(turbocfd_project_options
        INTERFACE
            MPI::MPI_CXX
    )
endif()

# Useful compile definitions visible project-wide through this interface target
target_compile_definitions(turbocfd_project_options
    INTERFACE
        $<$<BOOL:${TURBOCFD_ENABLE_MPI}>:TURBOCFD_USE_MPI=1>
        $<$<BOOL:${TURBOCFD_ENABLE_OPENMP}>:TURBOCFD_USE_OPENMP=1>
        $<$<BOOL:${TURBOCFD_ENABLE_PETSC}>:TURBOCFD_USE_PETSC=1>
        $<$<BOOL:${TURBOCFD_ENABLE_CGNS}>:TURBOCFD_USE_CGNS=1>
)

# ------------------------------------------------------------------------------
# Interface target for warnings
# ------------------------------------------------------------------------------

add_library(turbocfd_warnings INTERFACE)
add_library(TurboCFD::Warnings ALIAS turbocfd_warnings)

if(TURBOCFD_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(turbocfd_warnings INTERFACE
            /W4
            /permissive-
            /EHsc
            /Zc:__cplusplus
            /Zc:preprocessor
            /utf-8
            $<$<BOOL:${TURBOCFD_ENABLE_WERROR}>:/WX>
        )
    else()
        target_compile_options(turbocfd_warnings INTERFACE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Woverloaded-virtual
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            $<$<BOOL:${TURBOCFD_ENABLE_WERROR}>:-Werror>
        )
    endif()
endif()

# ------------------------------------------------------------------------------
# Sanitizers
# ------------------------------------------------------------------------------

add_library(turbocfd_sanitizers INTERFACE)
add_library(TurboCFD::Sanitizers ALIAS turbocfd_sanitizers)

if(TURBOCFD_ENABLE_SANITIZERS)
    if(NOT MSVC)
        target_compile_options(turbocfd_sanitizers INTERFACE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )
        target_link_options(turbocfd_sanitizers INTERFACE
            -fsanitize=address,undefined
        )
    else()
        message(WARNING "Sanitizers requested, but this file does not enable them for MSVC")
    endif()
endif()

# ------------------------------------------------------------------------------
# Version/config header
# ------------------------------------------------------------------------------

configure_file(
    "${CMAKE_SOURCE_DIR}/include/TurboCFD/Core/Version.h.in"
    "${CMAKE_BINARY_DIR}/generated/TurboCFD/Core/Version.h"
    @ONLY
)

target_include_directories(turbocfd_project_options
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
)

# ------------------------------------------------------------------------------
# Subdirectories
# ------------------------------------------------------------------------------

add_subdirectory(src)

if(TURBOCFD_ENABLE_APPS AND EXISTS "${CMAKE_SOURCE_DIR}/apps/CMakeLists.txt")
    add_subdirectory(apps)
endif()

if(TURBOCFD_ENABLE_TESTS)
    include(CTest)
    enable_testing()
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt")
        add_subdirectory(tests)
    endif()
endif()

if(TURBOCFD_ENABLE_BENCHMARKS AND EXISTS "${CMAKE_SOURCE_DIR}/benchmarks/CMakeLists.txt")
    add_subdirectory(benchmarks)
endif()

# ------------------------------------------------------------------------------
# Install
# ------------------------------------------------------------------------------

include(GNUInstallDirs)

install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
    DIRECTORY "${CMAKE_BINARY_DIR}/generated/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# ------------------------------------------------------------------------------
# Summary
# ------------------------------------------------------------------------------

message(STATUS "")
message(STATUS "============================================================")
message(STATUS "TurboCFD configuration summary")
message(STATUS "------------------------------------------------------------")
message(STATUS "Project version          : ${PROJECT_VERSION}")
message(STATUS "Build type               : ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard             : ${CMAKE_CXX_STANDARD}")
message(STATUS "Build shared libs        : ${BUILD_SHARED_LIBS}")
message(STATUS "Enable apps              : ${TURBOCFD_ENABLE_APPS}")
message(STATUS "Enable tests             : ${TURBOCFD_ENABLE_TESTS}")
message(STATUS "Enable benchmarks        : ${TURBOCFD_ENABLE_BENCHMARKS}")
message(STATUS "Enable MPI               : ${TURBOCFD_ENABLE_MPI}")
message(STATUS "Enable OpenMP            : ${TURBOCFD_ENABLE_OPENMP}")
message(STATUS "Enable PETSc             : ${TURBOCFD_ENABLE_PETSC}")
message(STATUS "Enable CGNS              : ${TURBOCFD_ENABLE_CGNS}")
message(STATUS "Enable warnings          : ${TURBOCFD_ENABLE_WARNINGS}")
message(STATUS "Warnings as errors       : ${TURBOCFD_ENABLE_WERROR}")
message(STATUS "Enable sanitizers        : ${TURBOCFD_ENABLE_SANITIZERS}")
message(STATUS "Enable IPO/LTO           : ${TURBOCFD_ENABLE_LTO}")
message(STATUS "============================================================")
message(STATUS "")