#pragma once

#include <cubes/editor/export.hpp>

struct GLFWwindow;

namespace cubes
{
    struct CUBES_EDITOR_API ApplicationCallbacks
    {
        static void on_mouse_pos    (GLFWwindow *window, double xpos,     double ypos);
        static void on_mouse_scroll (GLFWwindow *window, double xoffset,  double yoffset);
        static void on_mouse_button (GLFWwindow *window, int button,            int action, int mods);
        static void on_keyboard     (GLFWwindow *window, int key, int scancode, int action, int mods);
        static void on_window_resize        (GLFWwindow *window, int xsize, int ysize);
        static void on_framebuffer_resize   (GLFWwindow *window, int xsize, int ysize);

        static void on_content_drop(GLFWwindow *window, int path_count, const char **paths);
    };
}
