#include <cubes/core/glutil/shader_program.hpp>
#include <cubes/core/glutil/helper.inl>
#include <cubes/core/opengl.hpp>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

namespace
{
}

namespace cubes
{
    OpenGLShader::OpenGLShader()
    {
        m_render_id = glCreateProgram();
    }

    OpenGLShader::~OpenGLShader()
    {
        if (glIsProgram(m_render_id))
        {
            glDeleteProgram(m_render_id);
        }

        m_cached_id =
        m_render_id = 0u;
    }

    OpenGLShader::OpenGLShader(const std::string &a_mixed_sources) : OpenGLShader()
    {
    #if defined(CUBES_DEBUG)

        if (a_mixed_sources.empty())
        {
            throw std::logic_error("cubes::OpenGLShader(const std::string &): empty source!");
        }

    #endif

        // string stream and line string for reading sources line-by-line
        std::stringstream stream(a_mixed_sources);  // !best performance.
        std::string line = "";

        // enumeration for easly indexing in arrays, What A Genius!
        enum {
            None = -1, VertexShader, FragmentShader, GeometryShader, CountShaders
        } in_read = None;

        bool available[CountShaders] =
        {
            false, false, false
        };

        std::string source[CountShaders] =
        {
            "", "", ""
        };

        while (std::getline(stream, line))
        {
            if (line.find("#section ") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                {
                    available[VertexShader] = true;
                    in_read = VertexShader;
                }

                else if (line.find("fragment") != std::string::npos)
                {
                    available[FragmentShader] = true;
                    in_read = FragmentShader;
                }

                else if (line.find("geometry") != std::string::npos)
                {
                    available[GeometryShader] = true;
                    in_read = GeometryShader;
                }

            #if defined(CUBES_DEBUG)

                else {
                    throw std::logic_error("#section ? shader type missing || not supported!");
                }

            #endif
                
                continue;
            }

            else {
                if (in_read == None)
                {
                    continue;
                }
            }

            if (source[in_read].length() != 0)
                source[in_read] += '\n' + line;
            else {
                source[in_read] += line;
            }
        }

    #if defined(CUBES_DEBUG)

        const CompilationStatus status =

    #endif

        compile_and_link(
            available[VertexShader]     ? source[VertexShader].c_str()    : nullptr,
            available[FragmentShader]   ? source[FragmentShader].c_str()  : nullptr,
            available[GeometryShader]   ? source[GeometryShader].c_str()  : nullptr
        );

    #if defined(CUBES_DEBUG)

        switch (status)
        {
        case CompilationStatus::CompilationError:
            throw std::runtime_error("cubes::OpenGLShader(const std::string &): compilation error!");

        case CompilationStatus::LinkingError:
            throw std::runtime_error("cubes::OpenGLShader(const std::string &): linking error!");

        default:break;
        }

    #endif
    }

    OpenGLShader::OpenGLShader(const std::string &a_vertex_source, const std::string &a_fragment_source) : OpenGLShader()
    {
    #if defined(CUBES_DEBUG)

        const CompilationStatus status =

    #endif

        compile_and_link(a_vertex_source.c_str(), a_fragment_source.c_str(), nullptr);

    #if defined(CUBES_DEBUG)

        switch (status)
        {
        case CompilationStatus::CompilationError:
            throw std::runtime_error("cubes::OpenGLShader(const std::string &, const std::string &): compilation error!");

        case CompilationStatus::LinkingError:
            throw std::runtime_error("cubes::OpenGLShader(const std::string &, const std::string &): linking error!");

        default:break;
        }

    #endif
    }

    OpenGLShader::OpenGLShader(const std::string &a_vertex_source, const std::string &a_fragment_source, const std::string &a_geometry_source) : OpenGLShader()
    {
    #if defined(CUBES_DEBUG)

        const CompilationStatus status =

    #endif

        compile_and_link(a_vertex_source.c_str(), a_fragment_source.c_str(), a_geometry_source.c_str());

    #if defined(CUBES_DEBUG)

        switch (status)
        {
        case CompilationStatus::CompilationError:
            throw std::runtime_error("cubes::OpenGLShader(const std::string &, const std::string &, const std::string &): compilation error!");

        case CompilationStatus::LinkingError:
            throw std::runtime_error("cubes::OpenGLShader(const std::string &, const std::string &, const std::string &): linking error!");

        default:break;
        }

    #endif
    }

    int32_t OpenGLShader::uniform(const std::string &a_uniform_name) const
    {
        if (m_cached_uniforms.find(a_uniform_name) != m_cached_uniforms.end())
        {
            return m_cached_uniforms[a_uniform_name];
        }

        const int32_t uniform_location = glGetUniformLocation(m_render_id, a_uniform_name.c_str());

        if (uniform_location != -1)
        {
            m_cached_uniforms[a_uniform_name] = uniform_location;
        }

        return uniform_location;
    }

    void OpenGLShader::attach() const
    {
        const uint32_t crnt_bound = u32_value(GL_CURRENT_PROGRAM);

        if (crnt_bound == m_render_id)
        {
            m_detach_is_safe = false;
            return;
        }

        m_cached_id = crnt_bound;
        glUseProgram(m_render_id);
    }

    void OpenGLShader::detach() const
    {
        if (u32_value(GL_CURRENT_PROGRAM) != m_render_id)
        {
            return;
        }

        if (m_detach_is_safe)
        {
            glUseProgram(m_cached_id);
            m_cached_id = 0u;
        }

        m_detach_is_safe = true;
    }

    uint32_t OpenGLShader::get_handle() const
    {
        return m_render_id;
    }

    OpenGLShader::CompilationStatus OpenGLShader::compile_and_link(const char *a_vert_src, const char *a_frag_src, const char *a_geom_src)
    {
    #if defined(CUBES_DEBUG)

        bool vert_compiled = true, frag_compiled = true, geom_compiled = true;

    #endif

        auto compile_shader = [&](const char *a_src, uint32_t a_type)
        {
            if (!a_src)
            {
                return 0u;
            }

            const uint32_t shader_id = glCreateShader(a_type);

        #if defined(CUBES_DEBUG)

            if (!shader_id)
            {
            }

        #endif

            glShaderSource(shader_id, 1, &a_src, 0);
            glCompileShader(shader_id);

        #if defined(CUBES_DEBUG)

            int32_t compiled = 0;
            glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);

            if (a_type == GL_VERTEX_SHADER)
            {
                vert_compiled = static_cast<bool>(compiled);
            }

            else if (a_type == GL_FRAGMENT_SHADER) { frag_compiled = static_cast<bool>(compiled); }
            else if (a_type == GL_GEOMETRY_SHADER) { geom_compiled = static_cast<bool>(compiled); }

            if (!compiled)
            {
            }

        #endif

            return shader_id;
        };

        uint32_t vert_id = compile_shader(a_vert_src, GL_VERTEX_SHADER);
        uint32_t frag_id = compile_shader(a_frag_src, GL_FRAGMENT_SHADER);
        uint32_t geom_id = compile_shader(a_geom_src, GL_GEOMETRY_SHADER);

    #if defined(CUBES_DEBUG)

        if (!vert_compiled || !frag_compiled || !geom_compiled)
        {
            return CompilationStatus::CompilationError;
        }

    #endif

        if (vert_id) { glAttachShader(m_render_id, vert_id); }
        if (frag_id) { glAttachShader(m_render_id, frag_id); }
        if (geom_id) { glAttachShader(m_render_id, geom_id); }

            glLinkProgram(m_render_id);

    #if defined(CUBES_DEBUG)

        int32_t linked = 0;
        glGetProgramiv(m_render_id, GL_LINK_STATUS, &linked);

        if (!linked)
        {
        }

    #endif

        if (geom_id) { glDetachShader(m_render_id, geom_id); }
        if (frag_id) { glDetachShader(m_render_id, frag_id); }
        if (vert_id) { glDetachShader(m_render_id, vert_id); }

        if (geom_id) { glDeleteShader(geom_id); }
        if (frag_id) { glDeleteShader(frag_id); }
        if (vert_id) { glDeleteShader(vert_id); }

    #if defined(CUBES_DEBUG)

        if (!linked)
        {
            return CompilationStatus::LinkingError;
        }

    #endif

        return CompilationStatus::Successful;
    }
}
