#pragma once

#include <cubes/export.hpp>

#if defined(CUBES_CORE_API_SHARED)

    #define CUBES_CORE_API_EXPORT CUBES_API_EXPORT
    #define CUBES_CORE_API_IMPORT CUBES_API_IMPORT

#else

    #define CUBES_CORE_API_EXPORT
    #define CUBES_CORE_API_IMPORT

#endif


#if defined(CUBES_CORE_API_CLIENT)

    #define CUBES_CORE_API CUBES_CORE_API_IMPORT

#else

    #define CUBES_CORE_API CUBES_CORE_API_EXPORT

#endif
