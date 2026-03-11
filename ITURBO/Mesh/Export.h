#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(TURBOCFD_MESH_BUILD)
#define TURBOCFD_MESH_API __declspec(dllexport)
#else
#define TURBOCFD_MESH_API __declspec(dllimport)
#endif
#else
#define TURBOCFD_MESH_API
#endif