
#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(ITURBO_CORE_BUILD)
#define ITURBO_CORE_API __declspec(dllexport)
#else
#define ITURBO_CORE_API __declspec(dllimport)
#endif
#else
#define ITURBO_CORE_API
#endif