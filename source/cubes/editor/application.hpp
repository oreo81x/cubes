#pragma once

#include <cubes/editor/export.hpp>
#include <cubes/core/camera/view.hpp>
#include <cubes/core/glutil/framebuffer.hpp>
#include <cubes/core/glutil/shader_program.hpp>
#include <cubes/core/glutil/vertex_array.hpp>
#include <cubes/core/glutil/texture.hpp>

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;

namespace cubes
{
    class CUBES_EDITOR_API Application
    {
    public:
        static inline Application *get()
        {
            static Application application;
            return &application;
        }

        int32_t main();

         Application();
        ~Application();

    private:
        void update();
        void render();

    private:
        glm::uvec2  m_editor_window_size        = glm::uvec2(0u, 0u);
        glm::uvec2  m_default_framebuffer_size  = glm::uvec2(0u, 0u);

        // main window for our very good and well made editor!
        GLFWwindow *m_editor_window = create_editor_window();
        bool        m_fullscreen_on = false;

        OpenGLTexture m_texture_pack = OpenGLTexture("assets/texture_packs/default-pack.png");
        OpenGLFramebuffer m_scene_fb = OpenGLFramebuffer(glm::uvec2(1.0f, 1.0f), 6u);

        OpenGLShader m_scene_shader = OpenGLShader(
            #include <cubes/editor/shaders/entity.glsl>
        );

        OpenGLShader m_basic_shader = OpenGLShader(
            #include <cubes/editor/shaders/cursor.glsl>
        );

        OpenGLVertexArray m_cube_va_lines = OpenGLVertexArray(
            #include <cubes/editor/objects/line_based_cube.data>
        );

        OpenGLVertexArray m_cube_va = OpenGLVertexArray(
            #include <cubes/editor/objects/triangle_based_cube.data>
        );

        View m_scene_view =
            View(glm::vec3(2.5f, 2.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f));

        glm::vec3 m_cursor_cube_center  = glm::vec3(0.0f, 0.0f, 0.0f);  // the cursor(, pointer || whatever) cube center
        glm::vec3 m_cursor_cube_color   = glm::vec3(0.0f, 1.0f, 0.0f);  // the cursor(, pointer || whatever) cube color

        int32_t m_controlled_cube = -1; // controlled object index, -1 if no object controlled
        uint32_t
            m_cubes_center_vb   = 0u, // vertex buffer that stores the centers(|| positions in other words) of placed cubes
            m_cubes_texture_vb  = 0u; // vertex buffer that stores the texture (virtual handle || id)

        std::vector<glm::vec3> m_cubes_center;
        std::vector<uint32_t> m_cubes_texture;

        uint32_t m_next_texture = 0u;

        enum class Action : uint8_t
        {
            None, RotateView, AddBlock, TranslateBlock, RemoveBlock
        } m_action = Action::None;

        float   m_crnt_frame = 0.0f;
        float   m_last_frame = 0.0f;
        float   m_delta_time = 0.0f;
        bool    m_control   = false;
        bool    m_display_inventory_window = false;

    private:
        #include <cubes/editor/application.inl>

    private:
        void keyboard();

        void switch_mode(); // switch fullscreen <-> windowed
        void imgui_setup(); // setup imgui api resources
        void imgui_clean(); // clean imgui api resources
        void docker_menu(); // default menu bar elements

        void docking_config(); // setup imgui docking
        void draw_the_scene(); // draws the scene to our custom framebuffer to be rendered on imgui scene window
        void on_imgui_stuff(); // imgui windows and it's elements

        // creates main window || viewport.
        GLFWwindow *create_editor_window();

    private:
        void exit();

        void undo();
        void redo();

    private:
        friend class ApplicationCallbacks;
    };
}
