#include <cubes/core/opengl.hpp>

namespace
{
    inline uint32_t u32_value(uint32_t a_gl_target_enum)
    {
        int32_t value = 0;
        glGetIntegerv(a_gl_target_enum, &value);

        return static_cast<uint32_t>(value);
    }
}
