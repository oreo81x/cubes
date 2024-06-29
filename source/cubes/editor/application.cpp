#include <cubes/editor/application.hpp>
#include <cubes/editor/callbacks.hpp>

// including cubes opengl wrapper module
#include <cubes/core/glutil/framebuffer.hpp>
#include <cubes/core/glutil/shader_program.hpp>
#include <cubes/core/glutil/vertex_array.hpp>

// including cubes opengl module
#include <cubes/core/opengl.hpp>

// including imgui module
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>

namespace
{
    glm::vec2 uv_map(glm::vec2 a_uv_map_sample, glm::vec2 a_sub_texture_size, glm::vec2 a_texture_size, glm::uvec2 a_choosen_texture)
    {
        const glm::vec2 minimum = glm::vec2(
            a_sub_texture_size.x * static_cast<float>(a_choosen_texture.x) / a_texture_size.x,
            a_sub_texture_size.y * static_cast<float>(a_choosen_texture.y) / a_texture_size.y
        );

        const glm::vec2 maximum = glm::vec2(
            a_sub_texture_size.x * static_cast<float>(a_choosen_texture.x + 1u) / a_texture_size.x,
            a_sub_texture_size.y * static_cast<float>(a_choosen_texture.y + 1u) / a_texture_size.y
        );

        a_uv_map_sample.x = !a_uv_map_sample.x ? minimum.x : maximum.x;
        a_uv_map_sample.y = !a_uv_map_sample.y ? minimum.y : maximum.y;

        return a_uv_map_sample;
    }

    struct MonitorData
    {
        GLFWmonitor *handle = nullptr;
        const GLFWvidmode *vidmode = nullptr;
        const glm::ivec2 pos = glm::ivec2(0);
    };

    inline glm::uvec2 initial_window_size(MonitorData a_monitor)
    {
        return glm::uvec2(
            static_cast<uint32_t>(static_cast<double>(a_monitor.vidmode->width)   * 0.80),
            static_cast<uint32_t>(static_cast<double>(a_monitor.vidmode->height)  * 0.80)
        );
    }

    inline glm::ivec2 initial_window_pos(MonitorData a_monitor, glm::uvec2 a_size = glm::uvec2(0u, 0u))
    {
        if (a_size == glm::uvec2(0u, 0u))
            a_size  = glm::uvec2(initial_window_size(a_monitor));

        return glm::ivec2(
            static_cast<int32_t>(static_cast<double>(a_monitor.pos.x) + (static_cast<double>(a_monitor.vidmode->width)   / 2.0) - (static_cast<double>(a_size.x) / 2.0)),
            static_cast<int32_t>(static_cast<double>(a_monitor.pos.y) + (static_cast<double>(a_monitor.vidmode->height)  / 2.0) - (static_cast<double>(a_size.y) / 2.0))
        );
    }

    inline bool operator<= (glm::ivec2 a_left, glm::ivec2 a_right) { return a_left.x <= a_right.x && a_left.y <= a_right.y; }
    inline bool operator>= (glm::ivec2 a_left, glm::ivec2 a_right) { return a_left.x >= a_right.x && a_left.y >= a_right.y; }

    inline MonitorData current_window_monitor(GLFWwindow *a_window = nullptr)
    {
        // getting connected monitors && count
        int32_t count_monitors = 0;
        GLFWmonitor **monitor = glfwGetMonitors(&count_monitors);

        // if connected monitors is 1, there is no need to do extra calcs then!
        if (count_monitors == 1)
        {
            return { monitor[0], glfwGetVideoMode(monitor[0]), glm::ivec2(0, 0) };
        }


    #if defined(CUBES_DEBUG)

        if (count_monitors == 0)
        {
            throw std::logic_error("in file(cubes/editor/application.cpp): <internal-linkage> current_window_monitor(GLFWwindow *): no monitor conected!");
        }

    #endif

        // if argument window is null we will make one implicitly
        const bool implicit_window = !a_window;
        glm::ivec2 window_size_pos = glm::ivec2(75, 75);
        
        if (implicit_window) // creating the implicit window
        {
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            a_window = glfwCreateWindow(window_size_pos.x, window_size_pos.y, "", nullptr, nullptr);
        }


    #if defined(CUBES_DEBUG)

        if (!a_window)
        {
            throw std::runtime_error("in file(cubes/editor/application.cpp): <internal-linkage> current_window_monitor(GLFWwindow *): cannot get valid window handle!");
        }

    #endif

        glm::ivec2 window_pos = glm::ivec2(0, 0);
        glfwGetWindowPos(a_window, &window_pos.x, &window_pos.y);

        if (implicit_window)
        {
            glfwDestroyWindow(a_window);
        }

        // now we can callit size pos!
        window_size_pos += window_pos;

        for (int32_t index = 0; index < count_monitors; index++)
        {
            glm::ivec2 monitor_pos = glm::ivec2(0, 0);
            glfwGetMonitorPos(monitor[index], &monitor_pos.x, &monitor_pos.y);

            const GLFWvidmode *vidmode = glfwGetVideoMode(monitor[index]);
            const glm::ivec2 monitor_size_pos = monitor_pos + glm::ivec2(vidmode->width, vidmode->height);

            if (window_pos >= monitor_pos && window_size_pos <= monitor_size_pos)
            {
                return { monitor[index], vidmode, monitor_pos };
            }
        }

    #if defined(CUBES_DEBUG)

        throw std::logic_error("in file(cubes/editor/application.cpp): <internal-linkage> current_window_monitor(GLFWwindow *): unknown monitor!");

    #endif

        return { nullptr, nullptr, glm::ivec2(0) };
    }

    inline bool make_window_visible(GLFWwindow *a_window)
    {
        glfwShowWindow(a_window);

        return static_cast<bool>(
            a_window
        );
    }
    
    enum LookingOrient {
        Forward = 1, Backward, Rightward, Leftward
    };

    inline LookingOrient looking_orient(glm::vec3 a_orientation)
    {
        return
            (a_orientation.z <=  1.0f && a_orientation.z >=  0.5f) ? Backward   :
            (a_orientation.z >= -1.0f && a_orientation.z <= -0.5f) ? Forward    :
            (a_orientation.x <=  1.0f && a_orientation.x >=  0.5f) ? Rightward  :
            (a_orientation.x >= -1.0f && a_orientation.x <= -0.5f) ? Leftward   : LookingOrient(0);
    }

    glm::dvec2 centered_mouse_offset(GLFWwindow *a_window, bool a_hold_condition)
    {
        static bool first_hold = true;

        if (a_hold_condition == true)
        {
            glm::ivec2 window_size = glm::ivec2(0, 0);
            glfwGetWindowSize   (a_window, &window_size.x, &window_size.y);
            glfwSetInputMode    (a_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            if (first_hold)
            {
                first_hold = !first_hold;
                glfwSetCursorPos(a_window, double(window_size.x) / 2.0, double(window_size.y) / 2.0);
            }

            glm::dvec2 offset = glm::dvec2(0.0, 0.0);
            glfwGetCursorPos(a_window, &offset.x, &offset.y);

            offset = glm::dvec2(
                (offset.x - (double(window_size.x) / 2.0)) / double(window_size.x),
                (offset.y - (double(window_size.y) / 2.0)) / double(window_size.y)
            );

            glfwSetCursorPos(a_window, double(window_size.x) / 2.0, double(window_size.y) / 2.0);

            return offset;
        }

        else if (a_hold_condition == false)
        {
            first_hold = true;
            glfwSetInputMode(a_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        return glm::dvec2(0.0, 0.0);
    }
}

namespace cubes
{
    int32_t Application::main()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);
        glEnable(GL_MULTISAMPLE);

        m_scene_shader.attach();
            glUniform2f(m_scene_shader.uniform("u_texture_size"), m_texture_pack.get_size().x, m_texture_pack.get_size().y);
            glUniform2f(m_scene_shader.uniform("u_sub_texture_size"), 16.0f, 16.0f);

        while (!glfwWindowShouldClose(m_editor_window))
        {
            update();
            render();

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        return 0;
    }

    Application::Application()
    {
        imgui_setup();

        glGenBuffers(2, &m_cubes_center_vb);
        m_cube_va.attach();
            m_cube_va.extern_link(m_cubes_center_vb,    VertexLayout(3u, 3u, sizeof(glm::vec3), 0u), 1u);
            m_cube_va.extern_link(m_cubes_texture_vb,   VertexLayout(4u, 1u, sizeof(uint32_t),  0u), 1u);
            m_cube_va.intern_link(VertexLayout(5u, 1u, sizeof(Vertex3D), offsetof(Vertex3D, face)));

        m_cubes_center.push_back(glm::ivec3(0, 0, 0));
        m_cubes_texture.push_back(0u);
    }

    Application::~Application()
    {
        glDeleteBuffers(2, &m_cubes_center_vb);
        m_cubes_center_vb   =
        m_cubes_texture_vb  = 0u;

        imgui_clean();

        if (m_editor_window)
        {
            glfwDestroyWindow(m_editor_window);
            m_editor_window = nullptr;
        }

        m_editor_window_size.x =
        m_editor_window_size.y = 0u;
    }

    void Application::update()
    {
        // [maybe] change this later...
        m_control = glfwGetKey(m_editor_window, GLFW_KEY_LEFT_ALT) || glfwGetKey(m_editor_window, GLFW_KEY_RIGHT_ALT);

        m_crnt_frame = glfwGetTime();
        m_delta_time = m_crnt_frame - m_last_frame;
        m_last_frame = m_crnt_frame;

        // keyboard... IDK what to say more dont be weird!
        keyboard();

        const glm::dvec2 mouse_offset =
            centered_mouse_offset(m_editor_window, m_control);

        if (m_control)
        {
            auto orient  = looking_orient(m_scene_view.orient());
            bool shifted = glfwGetKey(m_editor_window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(m_editor_window, GLFW_KEY_RIGHT_SHIFT);

            glm::vec2 almost_axis = glm::vec2(abs(mouse_offset.x) > abs(mouse_offset.y) ? mouse_offset.x : 0.0f, abs(mouse_offset.y) > abs(mouse_offset.x) ? mouse_offset.y : 0.0f);
            glm::vec3 move_offset = (
                (orient == Backward)  ? glm::vec3(-almost_axis.x, (shifted) ? -almost_axis.y : 0.0f, (!shifted) ? -almost_axis.y : 0.0f) :
                (orient == Forward)   ? glm::vec3( almost_axis.x, (shifted) ? -almost_axis.y : 0.0f, (!shifted) ?  almost_axis.y : 0.0f) :
                (orient == Rightward) ? glm::vec3((!shifted) ? -almost_axis.y : 0.0f, (shifted) ? -almost_axis.y : 0.0f,  almost_axis.x) :
                (orient == Leftward)  ? glm::vec3((!shifted) ?  almost_axis.y : 0.0f, (shifted) ? -almost_axis.y : 0.0f, -almost_axis.x) : glm::vec3(0.0f, 0.0f, 0.0f)
            );

            if (m_action != Action::RotateView)
            {
                m_cursor_cube_center += move_offset * 1.0f;
            }

            if (m_action == Action::TranslateBlock)
            {
                const int32_t id = m_controlled_cube;

                if (glm::ivec3(m_cubes_center[m_controlled_cube]) != glm::ivec3(m_cursor_cube_center) && select_block())
                    m_cursor_cube_color = glm::vec3(1.0f, 0.0f, 0.0f);

                else if (glm::ivec3(m_cubes_center[m_controlled_cube]) != glm::ivec3(m_cursor_cube_center) && !select_block())
                    m_cursor_cube_color = glm::vec3(0.0f, 1.0f, 0.0f);

                m_controlled_cube = id;
            }
        }

        switch (m_action)
        {
         case Action::RotateView:
            m_scene_view.rotate(
                glm::radians(glm::vec2(-mouse_offset.y, mouse_offset.x) * 35.0f)
            );
            break;

         case Action::AddBlock:
            add_block();
            break;

         case Action::TranslateBlock:
            translate_block();
            break;

         case Action::RemoveBlock:
            remove_block();

         default: break;
        }

        select_block();

        glfwSwapBuffers(m_editor_window);
        glfwWaitEventsTimeout(2.5);

        if (m_cubes_center.size())
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_cubes_center_vb);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_cubes_center.size(), m_cubes_center.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, m_cubes_texture_vb);
            glBufferData(GL_ARRAY_BUFFER, sizeof(uint32_t) * m_cubes_texture.size(), m_cubes_texture.data(), GL_STATIC_DRAW);
        }
    }

    void Application::render()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // drawing our UI (imgui windows, etc)
        on_imgui_stuff();

        // _______________________________________________________________________
        static const bool   // well, Iknow my style-convention is inspiring you...
                make_editor_window_visible = make_window_visible(m_editor_window);
        (void)  make_editor_window_visible;
    }

    void Application::keyboard()
    {
        if (!m_control)
        {
            return;
        }

        // I hate current keyboard control to be honest but GLFW's input system is aaHHHH IDK.
        const float speed = (glfwGetKey(m_editor_window, GLFW_KEY_LEFT_SHIFT) ? 7.5f : 2.5f) * m_delta_time;

        if (glfwGetKey(m_editor_window, GLFW_KEY_W)) { m_scene_view.move(speed *  m_scene_view.orient()); }
        if (glfwGetKey(m_editor_window, GLFW_KEY_A)) { m_scene_view.move(speed * -m_scene_view.rtward()); }
        if (glfwGetKey(m_editor_window, GLFW_KEY_S)) { m_scene_view.move(speed * -m_scene_view.orient()); }
        if (glfwGetKey(m_editor_window, GLFW_KEY_D)) { m_scene_view.move(speed *  m_scene_view.rtward()); }
    }

    void Application::switch_mode()
    {
        MonitorData monitor = current_window_monitor(m_editor_window);

        static glm::ivec2 cached_window_pos   = glm::ivec2(0, 0);
        static glm::ivec2 cached_window_size  = glm::ivec2(0, 0);

        if (!m_fullscreen_on)
        {
            glfwGetWindowPos    (m_editor_window, &cached_window_pos.x,   &cached_window_pos.y);
            glfwGetWindowSize   (m_editor_window, &cached_window_size.x,  &cached_window_size.y);

            glfwSetWindowMonitor(m_editor_window, monitor.handle, 0, 0, monitor.vidmode->width, monitor.vidmode->height, monitor.vidmode->refreshRate);
        }

        else {
            glfwSetWindowMonitor(m_editor_window, NULL, cached_window_pos.x, cached_window_pos.y, cached_window_size.x, cached_window_size.y, monitor.vidmode->refreshRate);
        }

        // switch fullscreen status [1 | 0]
        m_fullscreen_on = !m_fullscreen_on;
    }

    void Application::imgui_setup()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGuiIO &io = ImGui::GetIO();

        io.IniFilename  =
        io.LogFilename  = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        ImGui_ImplGlfw_InitForOpenGL(m_editor_window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }

    void Application::imgui_clean()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Application::docker_menu()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save")) {}
                if (ImGui::MenuItem("Open")) {}

                if (ImGui::MenuItem("Exit"))
                {
                    exit();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo")) { undo(); }
                if (ImGui::MenuItem("Redo")) { redo(); }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Fullscreen", "F11"))
                {
                    switch_mode();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("Donate"))
                {
                    // open my patreon account for example
                }

                if (ImGui::MenuItem("Manual"))
                {
                    // show user how to use ur software
                }

                if (ImGui::MenuItem("About"))
                {
                    // tell user about ur software and ur self
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    void Application::docking_config()
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        // reference1: https://gist.github.com/AidanSun05/953f1048ffe5699800d2c92b88c36d9f
        // reference2: https://gist.github.com/PossiblyAShrub/0aea9511b84c34e191eaa90dd7225969

        ImGuiViewport *viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGuiWindowFlags docker_window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockerWindow", nullptr, docker_window_flags);
        ImGui::PopStyleVar(1);
        ImGui::PopStyleVar(2);

        ImGuiID dockspace_id = ImGui::GetID("MainWindowGroup");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        static bool
            dockspace_not_configured = true;

        if (dockspace_not_configured)
        {
            dockspace_not_configured = false;

            ImGui::DockBuilderRemoveNode    (dockspace_id);
            ImGui::DockBuilderAddNode       (dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodePos    (dockspace_id, viewport->WorkPos);
            ImGui::DockBuilderSetNodeSize   (dockspace_id, viewport->WorkSize);

            ImGuiID dock1 = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right,  0.20f, nullptr, &dockspace_id);

            ImGui::DockBuilderDockWindow("Scene",       dockspace_id);
            ImGui::DockBuilderDockWindow("Attributes",  dock1);
            ImGui::DockBuilderFinish(dockspace_id);
        }

        docker_menu();
        ImGui::End();
    }

    void Application::draw_the_scene()
    {
        const glm::mat4x4 camera_matrix_view = m_scene_view.view_matrix();
        const glm::mat4x4 camera_matrix_clip = glm::perspective(
            glm::radians(55.0f), static_cast<float>(m_scene_fb.get_size().x) / static_cast<float>(m_scene_fb.get_size().y), 0.1f, 1000.0f
        );

        m_scene_fb.attach();
            glViewport(0, 0, m_scene_fb.get_size().x, m_scene_fb.get_size().y);
            m_texture_pack.attach();

            glClearColor(0.75f, 0.65f, 0.85f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glEnable(GL_DEPTH_TEST);
            m_scene_shader.attach();
                glUniformMatrix4fv(m_scene_shader.uniform("u_view"), 1, GL_FALSE, &camera_matrix_view[0][0]);
                glUniformMatrix4fv(m_scene_shader.uniform("u_clip"), 1, GL_FALSE, &camera_matrix_clip[0][0]);

                m_cube_va.attach();
                glDrawArraysInstanced(GL_TRIANGLES, 0, m_cube_va.count_vertices(), m_cubes_center.size());

            m_basic_shader.attach();
                glUniformMatrix4fv(m_basic_shader.uniform("u_view"), 1, GL_FALSE, &camera_matrix_view[0][0]);
                glUniformMatrix4fv(m_basic_shader.uniform("u_clip"), 1, GL_FALSE, &camera_matrix_clip[0][0]);
                glUniform3fv(m_basic_shader.uniform("u_color"),  1, &m_cursor_cube_color[0]);
                glUniform3fv(m_basic_shader.uniform("u_center"), 1, &glm::vec3((glm::ivec3)m_cursor_cube_center)[0]);

                m_cube_va_lines.attach();
                glDrawElements(GL_LINES, m_cube_va_lines.count_elements(), GL_UNSIGNED_INT, (const void *)0);

            glDisable(GL_DEPTH_TEST);
        m_scene_fb.detach();

        // restore old viewport
        glViewport(0, 0, m_default_framebuffer_size.x, m_default_framebuffer_size.y);
    }

    void Application::on_imgui_stuff()
    {
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();

        ImGui::NewFrame();

            docking_config();

            ImGuiWindowClass window_class;
            window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoUndocking;

            // Configuring Next Window Attributes...
            ImGui::SetNextWindowClass(&window_class);
            ImVec2 scene_window_pos, scene_window_size;

            if (ImGui::Begin("Scene", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
            {
                scene_window_pos = ImGui::GetWindowPos();
                scene_window_size = ImGui::GetWindowSize();

                m_scene_fb.resize(glm::uvec2(scene_window_size.x, scene_window_size.y));

                // offscreen render the scene on our framebuffer
                draw_the_scene();

                // render our framebuffer texture on our imgui scene window
                ImGui::Image(
                    (ImTextureID)static_cast<uint64_t>(m_scene_fb.get_texture()), scene_window_size, ImVec2{ 0.0f, 1.0f }, ImVec2{ 1.0f, 0.0f }
                );
            }
            ImGui::End();

            // Configuring Next Window Attributes...
            ImGui::SetNextWindowClass(&window_class);

            if (ImGui::Begin("Attributes", 0, ImGuiWindowFlags_NoCollapse))
            {
            }
            ImGui::End();

            if (m_display_inventory_window)
            {
                if (ImGui::Begin("Textures", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration))
                {
                    const ImVec2 button_size = ImVec2{ 64.0f, 64.0f };

                    if (ImGui::BeginTable("TextureTable", 8, ImGuiTableFlags_SizingFixedFit))
                    {
                        for (uint32_t texture_set = 0u; texture_set < m_texture_pack.get_size().y / 16u; texture_set++)
                        {
                            ImGui::TableNextColumn();

                            const bool select_this_texture = ImGui::ImageButton(
                                std::string("TextureSet" + std::to_string(texture_set)).c_str(),
                                ImTextureID((uint64_t)m_texture_pack.get_handle()), button_size,

                                // This casting considered unsafe but Technically speaking glm::vec2 and ImVec2 have identical signature.
                                *(ImVec2 *)&uv_map(glm::vec2(0.0f, 1.0f), glm::vec2(16.0f), m_texture_pack.get_size(), glm::uvec2(0u, texture_set))[0],
                                *(ImVec2 *)&uv_map(glm::vec2(1.0f, 0.0f), glm::vec2(16.0f), m_texture_pack.get_size(), glm::uvec2(0u, texture_set))[0]
                            );

                            if (select_this_texture)
                            {
                                change_block_texture_set(texture_set);
                            }

                            (void)select_this_texture;
                        }
                        ImGui::EndTable();
                    }

                    const ImVec2 window_size = ImGui::GetWindowSize();

                    ImGui::SetWindowPos(
                        ImVec2 {
                            scene_window_pos.x + (scene_window_size.x * 0.5f) - (window_size.x * 0.5f),
                            scene_window_pos.y + (scene_window_size.y * 0.5f) - (window_size.y * 0.5f)
                        }
                    );
                }
                ImGui::End();
            }

        ImGui::EndFrame();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    GLFWwindow *Application::create_editor_window()
    {
        MonitorData monitor = current_window_monitor();

        m_editor_window_size   = initial_window_size(monitor);
        glm::ivec2 window_pos  =
            initial_window_pos(monitor, m_editor_window_size);

        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, CUBES_CORE_OPENGL_MAJOR_VERSION);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, CUBES_CORE_OPENGL_MINOR_VERSION);

        glfwWindowHint(GLFW_VISIBLE,    GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED,  GLFW_FALSE);
        glfwWindowHint(GLFW_REFRESH_RATE, monitor.vidmode->refreshRate);

        glfwWindowHint(GLFW_POSITION_X, window_pos.x);
        glfwWindowHint(GLFW_POSITION_Y, window_pos.y);

        m_editor_window = glfwCreateWindow(m_editor_window_size.x, m_editor_window_size.y, "cubes-editor", nullptr, nullptr);

    #if defined(CUBES_DEBUG)

        if (!m_editor_window)
        {
            throw std::runtime_error("in file(cubes/editor/application.cpp): cubes::Application::create_editor_window(): window creation failed!");
        }

    #endif

        glfwDefaultWindowHints();
        glfwMakeContextCurrent(m_editor_window);

    #if defined(CUBES_DEBUG)

        const bool opengl_loaded =

    #endif

        gladLoadGL();

    #if defined(CUBES_DEBUG)

        if (!opengl_loaded)
        {
            throw std::runtime_error("in file(cubes/editor/application.cpp): cubes::Application::create_editor_window(): gladLoadGL() failed!");
        }

    #endif

        // enabling vsync...
        glfwSwapInterval(1);

        int32_t fb_size_x = 0, fb_size_y = 0;
        glfwGetFramebufferSize(m_editor_window, &fb_size_x, &fb_size_y);

        m_default_framebuffer_size.x = fb_size_x;
        m_default_framebuffer_size.y = fb_size_y;

        // setting opengl viewport initially...
        glViewport(0, 0, fb_size_x, fb_size_y);

        glfwSetWindowSizeLimits         (m_editor_window, m_editor_window_size.x, m_editor_window_size.y, monitor.vidmode->width, monitor.vidmode->height);
        glfwSetWindowSizeCallback       (m_editor_window, ApplicationCallbacks::on_window_resize);
        glfwSetFramebufferSizeCallback  (m_editor_window, ApplicationCallbacks::on_framebuffer_resize);

        glfwSetKeyCallback   (m_editor_window, ApplicationCallbacks::on_keyboard);
        glfwSetDropCallback  (m_editor_window, ApplicationCallbacks::on_content_drop);

        glfwSetScrollCallback       (m_editor_window, ApplicationCallbacks::on_mouse_scroll);
        glfwSetMouseButtonCallback  (m_editor_window, ApplicationCallbacks::on_mouse_button);
        glfwSetCursorPosCallback    (m_editor_window, ApplicationCallbacks::on_mouse_pos);

        glfwSetInputMode(m_editor_window, GLFW_LOCK_KEY_MODS,  GLFW_TRUE);
        glfwSetInputMode(m_editor_window, GLFW_STICKY_KEYS,    GLFW_TRUE);

        // yes iknow it looks very clean.
        return m_editor_window;
    }

    void Application::exit()
    {
        glfwHideWindow(m_editor_window);
        glfwSetWindowShouldClose(m_editor_window, GLFW_TRUE);
    }

    void Application::undo()
    {
    }

    void Application::redo()
    {
    }
}

// ______________________________________________________________________
// working in backend only stuff

namespace
{
    struct WindowApiServer
    {
        inline WindowApiServer()
        {
        #if defined(CUBES_DEBUG)

            const bool is_api_initialized =

        #endif

            glfwInit();

        #if defined(CUBES_DEBUG)

            if (!is_api_initialized)
            {
                throw std::runtime_error("in file(cubes/editor/application.cpp): <internal-linkage> WindowApiServer(): glfwInit() failed!");
            }

        #endif
        }

        inline ~WindowApiServer()
        {
            glfwTerminate();
        }
    } WindowApiServer;
}
