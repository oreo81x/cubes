workspace "cubes"
    location "output/build"

    targetdir   ("output/bin/"      .. "%{cfg.architecture}-%{cfg.system}-%{cfg.buildcfg}")
    objdir      ("output/bin-obj/"  .. "%{cfg.architecture}-%{cfg.system}-%{cfg.buildcfg}/%{prj.name}")
    
    newaction {
        trigger     = "clean",
        description = "clean generated build files and compiled binaries",
        execute     = function()
            os.rmdir("output")
            os.mkdir("output")
            os.mkdir("output/build")
            os.mkdir("output/bin")
            os.mkdir("output/bin-obj")
        end
    }
    
    platforms {
        "x86_64", "x86"
    }

    configurations {
        "debug", "release"
    }

    filter "platforms:x86_64"
        architecture "x86_64"
    filter "platforms:x86"
        architecture "x86"
    filter "configurations:debug"
        runtime "Debug"
        symbols "On"
    filter "configurations:release"
        runtime "Release"
        optimize "Speed"

        defines {
            "NDEBUG"
        }
    filter {}

    includedirs {
        "source", "extern"
    }

    warnings "Extra"
    language   "C++"
    cppdialect "C++14"
    staticruntime "On"
    systemversion "latest"

    -- IDK what this does
    floatingpoint "Fast"

    project "core"
        kind        "SharedLib"
        targetname  "cubes-core"

        files {
            "source/cubes/core/**.inl",
            "source/cubes/core/**.hpp",
            "source/cubes/core/**.cpp",
            "source/cubes/core/**.c",
            "source/cubes/core/**.h"
        }
        
        defines {
            "CUBES_CORE_API_SHARED", "GLM_ENABLE_EXPERIMENTAL"
        }

    project "editor"
        kind        "SharedLib"
        targetname  "cubes-editor"

        files {
            "source/cubes/editor/**.inl",
            "source/cubes/editor/**.hpp",
            "source/cubes/editor/**.cpp",
            "source/cubes/core/**.c",
            "source/cubes/core/**.h"
        }

        defines {
            "CUBES_EDITOR_API_SHARED",
            "CUBES_CORE_API_SHARED",
            "CUBES_CORE_API_CLIENT"
        }

        links {
            "core", "cubes-glfw", "X11"
        }

    project "executable"
        kind "ConsoleApp"
        targetname "cubes-editor"

        files {
            "source/cubes/executable.cpp"
        }

        defines {
            "CUBES_EDITOR_API_SHARED",
            "CUBES_EDITOR_API_CLIENT",
            "CUBES_CORE_API_SHARED",
            "CUBES_CORE_API_CLIENT"
        }

        links {
            "editor"
        }

    include "extern/GLFW"