project "ImGuizmo"
    kind "StaticLib"
    language "C++"

    objdir ("%{RootPath}/build/%{_ACTION}/Magma/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Magma/lib")

    files {
        "%{VendorPaths.ImGuizmo}/*.h",
        "%{VendorPaths.ImGuizmo}/*.cpp"
    }

    includedirs {
        "%{Includes.ImGuizmo}",
        "%{Includes.imgui}/imgui"
    }
