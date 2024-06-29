project "ImGui"
    kind "StaticLib"

    language "C++"
    warnings "Off"

    pic "On"

    targetdir   (externlibdir)
    objdir      (externobjdir)

    files {
        "*.hpp","*.cpp"
    }