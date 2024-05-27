#include <cubes/core/camera/view.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace
{
    constexpr glm::vec3 g_global_rtward = glm::vec3(1.0f, 0.0f,  0.0f);
    constexpr glm::vec3 g_global_upward = glm::vec3(0.0f, 1.0f,  0.0f);
    constexpr glm::vec3 g_global_orient = glm::vec3(0.0f, 0.0f, -1.0f);
}

namespace cubes
{
    glm::vec3 global_orient() { return g_global_orient; }
    glm::vec3 global_rtward() { return g_global_rtward; }
    glm::vec3 global_upward() { return g_global_upward; }
}

namespace cubes
{
    View:: View() = default;
    View::~View() = default;

    View::View(glm::vec3 a_center, glm::vec3 a_target)
        : m_center(a_center), m_orient(glm::normalize(a_target - a_center))
    {
    }

    void View::move(glm::vec3 a_offset)
    {
        m_center += a_offset;
    }

    void View::center(glm::vec3 a_center) { m_center = a_center; }
    void View::orient(glm::vec3 a_orient) { m_orient = a_orient; }
    void View::lookat(glm::vec3 a_target)
    {
        m_orient = glm::normalize(a_target - m_center);
    }

    void View::rotate(glm::vec2 a_rotate)
    {
        const glm::vec3 new_orient = glm::rotate(m_orient, a_rotate.x, rtward());

        if (!(glm::angle(new_orient, g_global_upward) <= glm::radians(5.0f) or glm::angle(new_orient, -g_global_upward) <= glm::radians(5.0f)))
        {
            m_orient = new_orient;
        }

        m_orient = glm::normalize(glm::rotate(m_orient, -a_rotate.y, upward()));
    }

    glm::vec3 View::center() const { return m_center; }
    glm::vec3 View::orient() const { return m_orient; }

    glm::vec3 View::rtward() const
    {
        return glm::normalize(glm::cross(m_orient, g_global_upward));
    }

    glm::vec3 View::upward() const
    {
        return glm::normalize(glm::cross(rtward(), m_orient));
    }

    glm::mat4x4 View::view_matrix() const
    {
        return glm::lookAtRH(m_center, m_center + m_orient, upward());
    }
}
