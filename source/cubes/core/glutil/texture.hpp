#pragma once

#include <cubes/core/export.hpp>
#include <glm/glm.hpp>
#include <string>

namespace cubes
{
    class CUBES_CORE_API OpenGLTexture
    {
    public:
         OpenGLTexture(const std::string &a_image_file);
        ~OpenGLTexture();

        glm::uvec2 get_size() const
        {
            return m_size;
        }

        void attach() const;
        void detach() const;

        uint32_t get_handle() const;

    private:
        mutable bool m_detach_is_safe = true;
        mutable uint32_t m_cached_id = 0u;
                uint32_t m_render_id = 0u;

        glm::uvec2 m_size = glm::uvec2(0u, 0u);
    };
}
