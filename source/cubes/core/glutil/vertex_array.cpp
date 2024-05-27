#include <cubes/core/glutil/vertex_array.hpp>
#include <cubes/core/glutil/helper.inl>
#include <cubes/core/opengl.hpp>

namespace
{
}

namespace cubes
{
    Vertex3D::Vertex3D(glm::vec3 a_position, glm::vec3 a_normal, glm::vec2 a_uv_map, uint32_t a_face)
        : position(a_position), normal(a_normal), uv_map(a_uv_map), face(a_face)
    {
    }

    VertexLayout::VertexLayout(uint32_t a_index, uint32_t a_count, uint32_t a_stride, uint64_t a_offset)
        : index(a_index), count(a_count), stride(a_stride), offset(a_offset)
    {
    }

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glGenVertexArrays(1, &m_render_id);
    }
    
    OpenGLVertexArray::~OpenGLVertexArray()
    {
        if (m_render_id)
            glDeleteVertexArrays(1, &m_render_id);

        if (m_vb_handle) { glDeleteBuffers(1, &m_vb_handle); }
        if (m_eb_handle) { glDeleteBuffers(1, &m_eb_handle); }

        m_count_vertices =
        m_count_elements =
        m_cached_id =
        m_render_id =
        m_vb_handle =
        m_eb_handle = 0u;
    }

    OpenGLVertexArray::OpenGLVertexArray(const std::vector<Vertex3D> &a_vertex_data)
        : OpenGLVertexArray(a_vertex_data.data(), a_vertex_data.size() * sizeof(Vertex3D), a_vertex_data.size(), nullptr, 0u)
    {
        attach();
            intern_link(VertexLayout(0u, 3u, sizeof(Vertex3D), offsetof(Vertex3D, position)));
            intern_link(VertexLayout(1u, 3u, sizeof(Vertex3D), offsetof(Vertex3D, normal)));
            intern_link(VertexLayout(2u, 2u, sizeof(Vertex3D), offsetof(Vertex3D, uv_map)));
        detach();
    }

    OpenGLVertexArray::OpenGLVertexArray(const std::vector<Vertex3D> &a_vertex_data, const std::vector<uint32_t> &a_element_data)
        : OpenGLVertexArray(a_vertex_data.data(), a_vertex_data.size() * sizeof(Vertex3D), a_vertex_data.size(), a_element_data.data(), a_element_data.size() * sizeof(uint32_t))
    {
        attach();
            intern_link(VertexLayout(0u, 3u, sizeof(Vertex3D), offsetof(Vertex3D, position)));
            intern_link(VertexLayout(1u, 3u, sizeof(Vertex3D), offsetof(Vertex3D, normal)));
            intern_link(VertexLayout(2u, 2u, sizeof(Vertex3D), offsetof(Vertex3D, uv_map)));
        detach();
    }

    OpenGLVertexArray::OpenGLVertexArray(const std::vector<glm::vec3> &a_vertex_data)
        : OpenGLVertexArray(a_vertex_data.data(), a_vertex_data.size() * sizeof(glm::vec3), a_vertex_data.size(), nullptr, 0u)
    {
        intern_link(VertexLayout(0u, 3u, sizeof(glm::vec3), 0u));
    }

    OpenGLVertexArray::OpenGLVertexArray(const std::vector<glm::vec3> &a_vertex_data, const std::vector<uint32_t> &a_element_data)
        : OpenGLVertexArray(a_vertex_data.data(), a_vertex_data.size() * sizeof(glm::vec3), a_vertex_data.size(), a_element_data.data(), a_element_data.size() * sizeof(uint32_t))
    {
        intern_link(VertexLayout(0u, 3u, sizeof(glm::vec3), 0u));
    }

    OpenGLVertexArray::OpenGLVertexArray(const void *a_vertex_data, uint32_t a_vertex_data_size, uint32_t a_vertex_count, const uint32_t *a_element_data, uint32_t a_element_data_size) : OpenGLVertexArray()
    {
        glGenBuffers(a_element_data_size > 0u ? 2 : 1, &m_vb_handle);
        attach();
            glBindBuffer(GL_ARRAY_BUFFER, m_vb_handle);
            glBufferData(GL_ARRAY_BUFFER, a_vertex_data_size, a_vertex_data, GL_STATIC_DRAW);

            if (a_element_data_size)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eb_handle);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_element_data_size, a_element_data, GL_STATIC_DRAW);
            }
        detach();

        m_count_vertices = a_vertex_count;
        m_count_elements = a_element_data_size / sizeof(uint32_t);
    }

    void OpenGLVertexArray::attach() const
    {
        const uint32_t crnt_bound = u32_value(GL_VERTEX_ARRAY_BINDING);
        
        if (crnt_bound == m_render_id)
        {
            m_detach_is_safe = false;
            return;
        }

        m_cached_id = crnt_bound;
        glBindVertexArray(m_render_id);
    }

    void OpenGLVertexArray::detach() const
    {
        if (u32_value(GL_VERTEX_ARRAY_BINDING) != m_render_id)
        {
            return;
        }

        if (m_detach_is_safe)
        {
            glBindVertexArray(m_cached_id);
            m_cached_id = 0u;
        }

        m_detach_is_safe = true;
    }

    void OpenGLVertexArray::extern_link(uint32_t a_vb_handle, VertexLayout a_layout, uint32_t a_divisor) const
    {
        attach();
            glBindBuffer(GL_ARRAY_BUFFER, a_vb_handle);
            glEnableVertexAttribArray(a_layout.index);
            glVertexAttribDivisor(a_layout.index, a_divisor);
            glVertexAttribPointer(a_layout.index, a_layout.count, GL_FLOAT, GL_FALSE, a_layout.stride, (const void *)a_layout.offset);
        detach();
    }

    void OpenGLVertexArray::intern_link(VertexLayout a_layout, uint32_t a_divisor) const
    {
        extern_link(m_vb_handle, a_layout, a_divisor);
    }

    uint32_t OpenGLVertexArray::count_vertices() const { return m_count_vertices; }
    uint32_t OpenGLVertexArray::count_elements() const { return m_count_elements; }

    uint32_t OpenGLVertexArray::get_handle() const
    {
        return m_render_id;
    }
}
