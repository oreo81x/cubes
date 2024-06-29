workspace "cubes"
    location "output/build/%{_TARGET_OS}/%{_ACTION}"

    externlibdir = "extern/lib/%{cfg.architecture}/%{cfg.system}"
    externbindir = "extern/bin/%{cfg.architecture}/%{cfg.system}"
    externobjdir = "extern/obj/%{cfg.architecture}-%{cfg.system}-%{cfg.buildcfg}/%{prj.name}"

    projectoutdir = "output/bin/%{cfg.architecture}-%{cfg.system}-%{cfg.buildcfg}"
    projectobjdir = "output/bin-obj/%{cfg.architecture}-%{cfg.system}-%{cfg.buildcfg}/%{prj.name}"

    targetdir   (projectoutdir)
    objdir      (projectobjdir)
    
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

    configurations {
        "debug", "release"
    }

    architecture "x86_64"

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
        "source", "extern/source"
    }

    libdirs (externlibdir)

    staticruntime "On"
    systemversion "latest"

    warnings "Extra"
    language   "C++"
    cppdialect "C++14"

    -- prefer Fastness over Accuracy
    floatingpoint "Fast"

    project "core"
        kind        "StaticLib"
        targetname  "cubes-core"

        files {
            "source/cubes/core/**.inl",
            "source/cubes/core/**.hpp",
            "source/cubes/core/**.cpp",
            "source/cubes/core/**.c",
            "source/cubes/core/**.h"
        }
        
        defines {
            --"CUBES_CORE_API_SHARED",
            "GLM_ENABLE_EXPERIMENTAL"
        }

    project "editor"
        kind        "StaticLib"
        targetname  "cubes-editor"

        files {
            "source/cubes/editor/**.inl",
            "source/cubes/editor/**.hpp",
            "source/cubes/editor/**.cpp",
            "source/cubes/editor/**.c",
            "source/cubes/editor/**.h"
        }

        defines {
            --"CUBES_EDITOR_API_SHARED",
            --"CUBES_CORE_API_SHARED",
            --"CUBES_CORE_API_CLIENT"
        }

    project "executable"
        kind "ConsoleApp"
        targetname "cubes-editor"

        files {
            "source/cubes/executable.cpp"
        }

        defines {
            --"CUBES_EDITOR_API_SHARED",
            --"CUBES_EDITOR_API_CLIENT"
        }

        links {
            "editor", "core", "ImGui", "GLFW"
        }

        filter "system:linux"

            links "X11"

        filter {  }

    include "extern/source/GLFW"
    include "extern/source/imgui"

workspace "cubes"
    startproject "executable"