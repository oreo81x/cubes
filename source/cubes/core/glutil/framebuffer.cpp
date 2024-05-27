#include <cubes/core/glutil/framebuffer.hpp>
#include <cubes/core/glutil/helper.inl>
#include <cubes/core/opengl.hpp>
#include <glm/glm.hpp>

namespace
{
}

namespace cubes
{
    OpenGLFramebufferMultisample::OpenGLFramebufferMultisample(glm::uvec2 a_framebuffer_size, uint32_t a_samples)
        : m_size(a_framebuffer_size), m_samples(a_samples)
    {
        const uint32_t cached_framebuffer_id    = u32_value(GL_FRAMEBUFFER_BINDING);
        const uint32_t cached_renderbuffer_id   = u32_value(GL_RENDERBUFFER_BINDING);
        const uint32_t cached_texture_multisample_id    = u32_value(GL_TEXTURE_BINDING_2D_MULTISAMPLE);
        const uint32_t cached_texture_id                = u32_value(GL_TEXTURE_BINDING_2D);

        glGenTextures(ArrayCount, m_texture_id);
        glBindTexture(GL_TEXTURE_2D,                m_texture_id[Resolved]);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,    m_texture_id[Multisampled]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_framebuffer_size.x, a_framebuffer_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, a_samples, GL_RGB, a_framebuffer_size.x, a_framebuffer_size.y, GL_TRUE);

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, cached_texture_multisample_id);
        glBindTexture(GL_TEXTURE_2D,             cached_texture_id);

        glGenRenderbuffers(1, &m_depth_stencil_renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depth_stencil_renderbuffer);

            glRenderbufferStorageMultisample(GL_RENDERBUFFER, a_samples, GL_DEPTH24_STENCIL8, a_framebuffer_size.x, a_framebuffer_size.y);

        glBindRenderbuffer(GL_RENDERBUFFER, cached_renderbuffer_id);

        glGenFramebuffers(ArrayCount, m_render_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_render_id[Multisampled]);

            glFramebufferTexture2D      (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_texture_id[Multisampled], 0);
            glFramebufferRenderbuffer   (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_stencil_renderbuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, m_render_id[Resolved]);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_id[Resolved], 0);

        glBindFramebuffer(GL_FRAMEBUFFER, cached_framebuffer_id);
    }

    OpenGLFramebufferMultisample::~OpenGLFramebufferMultisample()
    {
        glDeleteFramebuffers(ArrayCount, m_render_id);
        glDeleteTextures    (ArrayCount, m_texture_id);
        glDeleteRenderbuffers(1, &m_depth_stencil_renderbuffer);

        for (int i = 0; i < ArrayCount; i++)
        {
            m_render_id[i] = m_texture_id[i] = 0u;
        }

        m_size.x =
        m_size.y =
        m_depth_stencil_renderbuffer = m_samples = 0u;
    }

    uint32_t OpenGLFramebufferMultisample::get_texture() const
    {
        const uint32_t cached_id_read = u32_value(GL_READ_FRAMEBUFFER_BINDING);
        const uint32_t cached_id_draw = u32_value(GL_DRAW_FRAMEBUFFER_BINDING);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_render_id[Multisampled]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_render_id[Resolved]);
            glBlitFramebuffer(0, 0, m_size.x, m_size.y, 0, 0, m_size.x, m_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, cached_id_draw);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, cached_id_read);

        return m_texture_id[Resolved];
    }

    void OpenGLFramebufferMultisample::attach() const
    {
        if (u32_value(GL_FRAMEBUFFER_BINDING) == m_render_id[Multisampled])
        {
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_render_id[Multisampled]);
    }

    void OpenGLFramebufferMultisample::detach() const
    {
        if (u32_value(GL_FRAMEBUFFER_BINDING) != m_render_id[Multisampled])
        {
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0u);
    }

    void OpenGLFramebufferMultisample::resize(glm::uvec2 a_framebuffer_size)
    {
        m_size = a_framebuffer_size;

        const uint32_t cached_renderbuffer_id         = u32_value(GL_RENDERBUFFER_BINDING);
        const uint32_t cached_texture_multisample_id  = u32_value(GL_TEXTURE_BINDING_2D_MULTISAMPLE);
        const uint32_t cached_texture_id              = u32_value(GL_TEXTURE_BINDING_2D);
        
        glBindRenderbuffer(GL_RENDERBUFFER, m_depth_stencil_renderbuffer);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_samples, GL_DEPTH24_STENCIL8, m_size.x, m_size.y);
        glBindRenderbuffer(GL_RENDERBUFFER, cached_renderbuffer_id);

        glBindTexture(GL_TEXTURE_2D,                m_texture_id[Resolved]);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,    m_texture_id[Multisampled]);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.x, m_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, GL_RGB, m_size.x, m_size.y, GL_TRUE);

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, cached_texture_multisample_id);
        glBindTexture(GL_TEXTURE_2D,             cached_texture_id);
    }

    glm::uvec2 OpenGLFramebufferMultisample::get_size() const
    {
        return m_size;
    }
}
