project "ImGuiColorTextEdit"
    kind "StaticLib"
    language "C++"

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
