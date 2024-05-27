project "cubes-glfw"
    kind "StaticLib"

    language "C++"  -- in reallity GLFW written in c
    warnings "Off"  -- GLFW have alot of silly warnings

    targetdir   ("lib"      .. "/%{cfg.architecture}-%{cfg.system}-%{cfg.buildcfg}")
    objdir      ("lib-obj"  .. "/%{cfg.architecture}-%{cfg.system}-%{cfg.buildcfg}")

    source = "src"
    header = ""

    files {
        "%{header}/glfw3.h",
        "%{header}/glfw3native.h",
        "%{source}/internal.h",
        "%{source}/platform.h",
        "%{source}/mappings.h",
        "%{source}/context.c",
        "%{source}/init.c",
        "%{source}/input.c",
        "%{source}/monitor.c",
        "%{source}/platform.c",
        "%{source}/vulkan.c",
        "%{source}/window.c",
        "%{source}/egl_context.c",
        "%{source}/osmesa_context.c",
        "%{source}/null_platform.h",
        "%{source}/null_joystick.h",
        "%{source}/null_init.c",
        "%{source}/null_monitor.c",
        "%{source}/null_window.c",
        "%{source}/null_joystick.c"
    }

    filter "system:linux"
        pic "On"

        systemversion "latest"
        staticruntime "On"

        files {
            "%{source}/x11_init.c",
            "%{source}/x11_monitor.c",
            "%{source}/x11_window.c",
            "%{source}/xkb_unicode.c",
            "%{source}/posix_poll.c",
            "%{source}/posix_time.c",
            "%{source}/posix_thread.c",
            "%{source}/posix_module.c",
            "%{source}/glx_context.c",
            "%{source}/egl_context.c",
            "%{source}/osmesa_context.c",
            "%{source}/linux_joystick.c"
        }

        defines {
            "_GLFW_X11"
        }

    filter "system:windows"
        systemversion "latest"
        staticruntime "On"
    
        -- buildoptions{
        --     "/MT"
        -- }

        files {
            "%{source}/win32_init.c",
            "%{source}/win32_module.c",
            "%{source}/win32_joystick.c",
            "%{source}/win32_monitor.c",
            "%{source}/win32_time.h",
            "%{source}/win32_time.c",
            "%{source}/win32_thread.h",
            "%{source}/win32_thread.c",
            "%{source}/win32_window.c",
            "%{source}/wgl_context.c",
            "%{source}/egl_context.c",
            "%{source}/osmesa_context.c"
        }

        defines { 
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }
