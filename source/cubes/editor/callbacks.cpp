#include <cubes/editor/callbacks.hpp>
#include <cubes/editor/application.hpp>
#include <cubes/core/opengl.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#define IGNORE(variable) (void)variable

bool inline modifier(int32_t base, int32_t searching_for)
{
    return (base == searching_for || base == (searching_for | GLFW_MOD_CAPS_LOCK) || base == (searching_for | GLFW_MOD_NUM_LOCK) || base == (searching_for | GLFW_MOD_CAPS_LOCK | GLFW_MOD_NUM_LOCK));
}

bool inline find_modifier(int32_t base, int32_t searching_for)
{
    return base & searching_for;
}

void cubes::ApplicationCallbacks::on_mouse_pos(GLFWwindow *window, double xpos, double ypos)
{
    IGNORE(window);
    IGNORE(xpos);
    IGNORE(ypos);
}

void cubes::ApplicationCallbacks::on_mouse_scroll(GLFWwindow *window, double xoffset, double yoffset)
{
    Application::get()->m_scene_view.move(Application::get()->m_scene_view.orient() * float(yoffset * 0.5));
    Application::get()->m_scene_view.move(Application::get()->m_scene_view.rtward() * float(xoffset * 0.5));

    IGNORE(window);
    IGNORE(xoffset);
}

void cubes::ApplicationCallbacks::on_mouse_button(GLFWwindow *window, int button, int action, int mods)
{
    if (modifier(mods, GLFW_MOD_ALT))
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (action == GLFW_PRESS)
            {
                Application::get()->m_action = Application::Action::RotateView;
            }

            else {
                Application::get()->m_action = Application::Action::None;
            }
        }

        else if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                if (Application::get()->select_block())
                    Application::get()->m_action = Application::Action::TranslateBlock;

                else {
                    Application::get()->m_action = Application::Action::AddBlock;
                }
            }

            else {
                Application::get()->m_action = Application::Action::None;
            }
        }
    }

    else if (modifier(mods, GLFW_MOD_ALT | GLFW_MOD_SHIFT))
    {
        // Note: This code is duplicated so you could find a better solution.
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                if (Application::get()->select_block())
                    Application::get()->m_action = Application::Action::TranslateBlock;

                else {
                    Application::get()->m_action = Application::Action::AddBlock;
                }
            }

            else {
                Application::get()->m_action = Application::Action::None;
            }
        }
    }

    else if (modifier(mods, GLFW_MOD_ALT | GLFW_MOD_CONTROL))
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && glfwGetKey(Application::get()->m_editor_window, GLFW_KEY_R))
        {
            if (action == GLFW_PRESS)
            {
                Application::get()->m_action = Application::Action::RemoveBlock;
            }

            else {
                Application::get()->m_action = Application::Action::None;
            }
        }
    }

    else if (modifier(mods, GLFW_MOD_ALT | GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
    {
    }
    
    else {
        return;
    }

    IGNORE(window);
    IGNORE(action);
}

void cubes::ApplicationCallbacks::on_keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (modifier(mods, 0))
    {
        if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
            Application::get()->switch_mode();

        else if (key == GLFW_KEY_F2)
        {
            static bool repeated = false;

            if (!repeated && action == GLFW_REPEAT)
            {
                repeated = Application::get()->m_display_inventory_window = true;
            }

            else if (repeated && action == GLFW_RELEASE)
            {
                repeated = Application::get()->m_display_inventory_window = false;
            }

            else if (action == GLFW_PRESS)
            {
                Application::get()->m_display_inventory_window = !Application::get()->m_display_inventory_window;
            }
        }

        return;
    }

    else if (modifier(mods, GLFW_MOD_CONTROL))
    {
        if (key == GLFW_KEY_Q && action == GLFW_PRESS)
            Application::get()->exit();

        else if (key == GLFW_KEY_Z && action == GLFW_PRESS) { Application::get()->undo(); }
        else if (key == GLFW_KEY_Y && action == GLFW_PRESS) { Application::get()->redo(); }
    }

    IGNORE(window);
    IGNORE(scancode);
    IGNORE(mods);
}

void cubes::ApplicationCallbacks::on_window_resize(GLFWwindow *window, int xsize, int ysize)
{
    Application::get()->m_editor_window_size.x = xsize;
    Application::get()->m_editor_window_size.y = ysize;

    IGNORE(window);
}

void cubes::ApplicationCallbacks::on_framebuffer_resize(GLFWwindow *window, int xsize, int ysize)
{
    Application::get()->m_default_framebuffer_size.x = xsize;
    Application::get()->m_default_framebuffer_size.y = ysize;

    IGNORE(window);
}

void cubes::ApplicationCallbacks::on_content_drop(GLFWwindow *window, int path_count, const char **paths)
{
    IGNORE(window);
    IGNORE(path_count);
    IGNORE(paths);
}
