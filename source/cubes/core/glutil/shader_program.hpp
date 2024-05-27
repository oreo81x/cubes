#pragma once

#include <cubes/core/export.hpp>

#include <unordered_map>
#include <string>

namespace cubes
{
    class CUBES_CORE_API OpenGLShader
    {
    public:
         OpenGLShader();
        ~OpenGLShader();

        OpenGLShader(const std::string &a_mixed_sources);
        OpenGLShader(const std::string &a_vertex_source, const std::string &a_fragment_source);
        OpenGLShader(const std::string &a_vertex_source, const std::string &a_fragment_source, const std::string &a_geometry_source);

        int32_t uniform(const std::string &a_uniform_name) const;
        
        void attach() const;
        void detach() const;

        uint32_t get_handle() const;

    private:
        enum class CompilationStatus : uint8_t
        {
            Successful = 0u, CompilationError = 1u, LinkingError = 2u
        };

        mutable bool m_detach_is_safe = true;
        mutable std::unordered_map<std::string, int32_t> m_cached_uniforms;
        mutable
            uint32_t m_cached_id = 0u;
            uint32_t m_render_id = 0u;

    private:
        CompilationStatus compile_and_link(const char *, const char *, const char *);
    };
}
