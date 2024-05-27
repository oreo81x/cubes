#pragma once

#include <cstdint>

#if !defined(NDEBUG)

    #define CUBES_DEBUG

#endif

#if defined(_WIN32)

    #define CUBES_API_EXPORT __declspec(dllexport)
    #define CUBES_API_IMPORT __declspec(dllimport)

#elif defined(__GNUC__) && __GNUC__ >= 4

    #define CUBES_API_EXPORT __attribute__((__visibility__("default")))
    #define CUBES_API_IMPORT __attribute__((__visibility__("default")))

#else

    #define CUBES_API_EXPORT
    #define CUBES_API_IMPORT

#endif
