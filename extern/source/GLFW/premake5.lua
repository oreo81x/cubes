project "GLFW"
    kind "StaticLib"

    language "C++"  -- in reallity GLFW written in c
    warnings "Off"  -- GLFW have alot of silly warnings

    pic "On" -- position independent code..

    targetdir   (externlibdir)
    objdir      (externobjdir)

    source = "src"
    header = ""

    files {
        "%{source}/init.c",

        "%{header}/glfw3.h",
        "%{header}/glfw3native.h",

        "%{source}/internal.h",
        "%{source}/mappings.h",
        "%{source}/platform.h",
        "%{source}/platform.c",

        "%{source}/input.c",
        "%{source}/monitor.c",
        "%{source}/context.c",
        "%{source}/vulkan.c",
        "%{source}/window.c",
        "%{source}/egl_context.c",
        "%{source}/osmesa_context.c",

        "%{source}/null_*.h",
        "%{source}/null_*.c"
    }

    filter "system:linux"

        -- not tested but should work
        files {
            "%{source}/x11_*.c", "%{source}/xkb_*.c", "%{source}/wl_*.c", "%{source}/linux_*.c", "%{source}/posix_*.c", "%{source}/glx_*.c",
            "%{source}/x11_*.h", "%{source}/xkb_*.h", "%{source}/wl_*.h", "%{source}/linux_*.h", "%{source}/posix_*.h", "%{source}/glx_*.h"
        }

        defines {
            "_GLFW_X11", "_GLFW_WAYLAND"
        }

    -- not supported yet.. I don't know required files.
    filter "system:macosx"

        -- not tested but should not work
        files {
            --"%{source}/_.c",
            --"%{source}/_.h"
        }

        defines "_GLFW_COCOA"

    filter "system:windows"

        -- buildoptions{
        --     "/MT"
        -- }
    
        -- not tested but should work
        files {
            "%{source}/win32_*.c", "%{source}/wgl_*.c",
            "%{source}/win32_*.h", "%{source}/wgl_*.h"
        }

        defines { 
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter {  }