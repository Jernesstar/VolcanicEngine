project "ImGuiFileDialog"
    kind "StaticLib"
    language "C++"

    objdir ("%{RootPath}/build/%{_ACTION}/Magma/obj")
    targetdir ("%{RootPath}/build/%{_ACTION}/Magma/lib")

    files {
        "%{VendorPaths.ImGuiFileDialog}/*.h",
        "%{VendorPaths.ImGuiFileDialog}/*.cpp"
    }

    includedirs {
        "%{Includes.ImGuiFileDialog}",
        "%{Includes.imgui}/imgui"
    }
