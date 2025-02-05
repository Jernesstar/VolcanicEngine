project "ImGuiFileDialog"
    kind "StaticLib"
    language "C++"

    objdir ("%{RootPath}/build/Magma/obj")
    targetdir ("%{RootPath}/build/Magma/lib")

    files {
        "%{VendorPaths.ImGuiFileDialog}/*.h",
        "%{VendorPaths.ImGuiFileDialog}/*.cpp"
    }

    includedirs {
        "%{Includes.ImGuiFileDialog}",
        "%{Includes.imgui}/imgui"
    }
