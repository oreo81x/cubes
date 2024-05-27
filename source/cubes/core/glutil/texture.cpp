#include <cubes/core/glutil/texture.hpp>
#include <cubes/core/glutil/helper.inl>
#include <cubes/core/stb/stb_image.h>
#include <cubes/core/opengl.hpp>

#include <glm/glm.hpp>
#include <stdexcept>
#include <string>

namespace
{
}

namespace cubes
{
    OpenGLTexture::OpenGLTexture(const std::string &a_image_file)
    {
        int width = 0, height = 0, format = 0;

        stbi_set_flip_vertically_on_load(1);
        unsigned char *pixels = stbi_load(a_image_file.c_str(), &width, &height, &format, 0);

        if (pixels)
        {
            glGenTextures(1, &m_render_id);

            format =
                (format == 1) ? GL_RED  :
                (format == 2) ? GL_RG   :
                (format == 3) ? GL_RGB  : GL_RGBA;

            attach();
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                glGenerateMipmap(GL_TEXTURE_2D);
            detach();
        }

    #if defined(CUBES_DEBUG)

        else {
            throw std::runtime_error(std::string("OpenGLTexture(const std::string &): failed load image file(") + a_image_file + ")!");
        }

    #endif

        stbi_image_free(pixels);
        pixels = nullptr;

        m_size.x = width;
        m_size.y = height;
    }

    OpenGLTexture::~OpenGLTexture()
    {
        if (m_render_id)
        {
            glDeleteTextures(1, &m_render_id);
        }

        m_size.x =
        m_size.y =
        m_cached_id =
        m_render_id = 0u;
    }

    void OpenGLTexture::attach() const
    {
        uint32_t crnt_bound = u32_value(GL_TEXTURE_BINDING_2D);

        if (crnt_bound == m_render_id)
        {
            m_detach_is_safe = false;
            return;
        }

        m_cached_id = crnt_bound;
        glBindTexture(GL_TEXTURE_2D, m_render_id);
    }

    void OpenGLTexture::detach() const
    {
        if (u32_value(GL_TEXTURE_BINDING_2D) != m_render_id)
        {
            return;
        }

        if (m_detach_is_safe)
        {
            glBindTexture(GL_TEXTURE_2D, m_cached_id);
            m_cached_id = 0u;
        }

        m_detach_is_safe = true;
    }

    uint32_t OpenGLTexture::get_handle() const
    {
        return m_render_id;
    }
}
