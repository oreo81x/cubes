#pragma once

#include <cubes/core/export.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace cubes
{
    enum {
        UpFace = 0, BottomFace, RightFace, LeftFace, FrontFace, BackFace
    };

    struct CUBES_CORE_API Vertex3D
    {
        glm::vec3 position  = glm::vec3(0.0f);
        glm::vec3 normal    = glm::vec3(0.0f);
        glm::vec2 uv_map    = glm::vec2(0.0f);
        uint32_t face = 0u;

        Vertex3D(glm::vec3 a_position, glm::vec3 a_normal, glm::vec2 a_uv_map, uint32_t a_face = 0u);
    };

    struct CUBES_CORE_API VertexLayout
    {
        uint32_t index  = 0u;   // layout index
        uint32_t count  = 0u;   // layout count components
        uint32_t stride = 0u;   // stride between each one
        uint64_t offset = 0u;   // offset for this layout.

        VertexLayout(uint32_t a_index, uint32_t a_count, uint32_t a_stride, uint64_t a_offset);
    };

    class CUBES_CORE_API OpenGLVertexArray
    {
    public:
         OpenGLVertexArray();
        ~OpenGLVertexArray();

        OpenGLVertexArray(const std::vector<Vertex3D> &a_vertex_data);
        OpenGLVertexArray(const std::vector<Vertex3D> &a_vertex_data, const std::vector<uint32_t> &a_element_data);

        OpenGLVertexArray(const std::vector<glm::vec3> &a_vertex_data);
        OpenGLVertexArray(const std::vector<glm::vec3> &a_vertex_data, const std::vector<uint32_t> &a_element_data);

        OpenGLVertexArray(const void *a_vertex_data, uint32_t a_vertex_data_size, uint32_t a_vertex_count, const uint32_t *a_element_data, uint32_t a_element_data_size);

        void attach() const;
        void detach() const;

        void extern_link(uint32_t a_vb_handle,  VertexLayout a_layout, uint32_t a_divisor = 0u) const;
        void intern_link(                       VertexLayout a_layout, uint32_t a_divisor = 0u) const;

        uint32_t count_vertices() const;
        uint32_t count_elements() const;

        uint32_t get_handle() const;

    private:
        mutable bool m_detach_is_safe = true;
        mutable
            uint32_t m_cached_id = 0u;  // cached gl vertex array handle from attach() call
            uint32_t m_render_id = 0u;  // gl vertex array handle

        uint32_t m_vb_handle = 0u;  // internal vertex buffer handle
        uint32_t m_eb_handle = 0u;  // internal element buffer handle

        uint32_t m_count_vertices = 0u;  // caching vertices count for rendering
        uint32_t m_count_elements = 0u;  // caching elements count for rendering
    };
}
