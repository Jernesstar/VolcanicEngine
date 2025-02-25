project "ImGuizmo"
    kind "StaticLib"
    language "C++"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.ImGuizmo}/*.h",
        "%{VendorPaths.ImGuizmo}/*.cpp"
    }

    includedirs {
        "%{Includes.ImGuizmo}",
        "%{Includes.imgui}/imgui"
    }
