#pragma once

#include <cubes/core/export.hpp>
#include <glm/glm.hpp>

namespace cubes
{
    class CUBES_CORE_API View
    {
    public:
         View();
        ~View();

        View(glm::vec3 a_center, glm::vec3 a_target);

        void move   (glm::vec3 a_offset);
        void center (glm::vec3 a_center);
        void orient (glm::vec3 a_orient);
        void lookat (glm::vec3 a_target);
        void rotate (glm::vec2 a_rotate);

        glm::vec3 center() const;
        glm::vec3 orient() const;

        glm::vec3 rtward() const;
        glm::vec3 upward() const;

        glm::mat4x4 view_matrix() const;

    protected:
        glm::vec3 m_center = glm::vec3(0.0f, 0.0f,  2.0f);
        glm::vec3 m_orient = glm::vec3(0.0f, 0.0f, -1.0f);
    };

    glm::vec3 global_orient();
    glm::vec3 global_rtward();
    glm::vec3 global_upward();
}
