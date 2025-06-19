project "ImGuiColorTextEdit"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    objdir ("%{RootPath}/build/%{_ACTION}/Magma/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Magma/lib")

    files {
        "%{VendorPaths.ImGuiColorTextEdit}/*.h",
        "%{VendorPaths.ImGuiColorTextEdit}/*.cpp"
    }

    includedirs {
        "%{Includes.ImGuiColorTextEdit}",
        "%{Includes.imgui}/imgui"
    }
