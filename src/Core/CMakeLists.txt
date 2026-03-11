# src/Core/CMakeLists.txt

file(GLOB ITURBO_CORE_PUBLIC_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/ITURBO/Core/*.h"
)

file(GLOB TURBOCFD_CORE_PRIVATE_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

file(GLOB TURBOCFD_CORE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(turbocfd_core
    ${TURBOCFD_CORE_PUBLIC_HEADERS}
    ${TURBOCFD_CORE_PRIVATE_HEADERS}
    ${TURBOCFD_CORE_SOURCES}
    ${CMAKE_SOURCE_DIR}/include/TurboCFD/Core/Version.h.in
)

add_library(TurboCFD::Core ALIAS turbocfd_core)

target_link_libraries(turbocfd_core
    PUBLIC
        TurboCFD::ProjectOptions
        TurboCFD::Warnings
        TurboCFD::Sanitizers
)

target_include_directories(turbocfd_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(turbocfd_core PUBLIC cxx_std_23)

set_target_properties(turbocfd_core PROPERTIES
    OUTPUT_NAME turbocfd_core
    EXPORT_NAME Core
)

install(
    TARGETS turbocfd_core
    EXPORT TurboCFDTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/TurboCFD/Core/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/TurboCFD/Core
    FILES_MATCHING PATTERN "*.h"
)

install(
    FILES
        ${CMAKE_BINARY_DIR}/generated/TurboCFD/Core/Version.h
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/TurboCFD/Core
)