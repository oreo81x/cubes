#pragma once

#include <cubes/core/export.hpp>
#include <glm/glm.hpp>

namespace cubes
{
    class CUBES_CORE_API OpenGLFramebufferMultisample
    {
    public:
         OpenGLFramebufferMultisample(glm::uvec2 a_framebuffer_size, uint32_t a_samples = 4u);
        ~OpenGLFramebufferMultisample();

        // returns Resolved(not-mulisampled) texture handle
        uint32_t get_texture() const;

        // ___________________________________________
        void attach() const;  // binds our framebuffer
        void detach() const;  // binds default framebuffer [maybe add caching like other OpenGL* classes later]

        // ________________________________________
        void resize(glm::uvec2 a_framebuffer_size);

        // _________________________
        // return framebuffer's size
        glm::uvec2 get_size() const;

    private:
        glm::uvec2 m_size = glm::uvec2(0u, 0u);  // our framebuffers size
        uint32_t m_samples = 0u;                 // MSAA number of samples

        enum {
            Resolved, Multisampled, ArrayCount
        };

        uint32_t m_render_id   [ArrayCount] { 0u };  // multisample framebuffer and resolved
        uint32_t m_texture_id  [ArrayCount] { 0u };  // multisample framebuffer and resolved color attachment textures

        // we need depth and stencil in multisampled framebuffer only
        uint32_t m_depth_stencil_renderbuffer = 0u;
    };


    // making this type name atlas to not destroy the world.
    using OpenGLFramebuffer = OpenGLFramebufferMultisample;
}
