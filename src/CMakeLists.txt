# src/CMakeLists.txt

set(ITURBO_SRC_MODULES
    Core
    Mesh
    Geometry
    Physics
    Numerics
    BC
    Turbomachinery
    LinearAlgebra
    Time
    Solver
    IO
    Post
    Utils
    Parallel
)

foreach(module IN LISTS TURBOCFD_SRC_MODULES)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${module}/CMakeLists.txt")
        add_subdirectory("${module}")
    else()
        message(STATUS "Skipping src/${module} (no CMakeLists.txt found yet)")
    endif()
endforeach()