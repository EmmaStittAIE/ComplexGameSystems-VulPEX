workspace "Hello Workspace"
    filename "HelloWorkspace"
    startproject "Test Application"

    language "C++"
    cppdialect "C++20"
    toolset "clang"

    location "generated"
    
    configurations { "Debug", "Release" }
    platforms { "Linux", "Win32", "Win64" }

    filter "configurations:Debug"
        symbols "On"
        optimize "Debug"
    
        defines { "_DEBUG" }

    filter "configurations:Release"
        symbols "Off"
        optimize "On"

        defines { "_RELEASE" }

    filter "platforms:Linux"
        system "linux"
        architecture "x86_64"

        defines { "_LINUX" }

    filter "platforms:Win32"
        system "windows"
        architecture "x86"

        defines { "_WIN32" }

    filter "platforms:Win64"
        system "windows"
        architecture "x86_64"

        defines { "_WIN64" }

    filter {}

project "Test Application"
    filename "TestApp"
    targetname "TestApp"

    includedirs
    {
        "lib/glm",
        "lib/GLFW/include"
    }

    libdirs
    {
        "lib/GLFW",
        libdirs { os.findlib("vulkan") }
    }

    links
    {
        "glfw3",
        "vulkan"
    }

    files
    {
        "src/TestApp/**.cpp",
    }

    filter "configurations:Debug"
        targetdir "build/debug/TestApp/bin"
        objdir "build/debug/TestApp/obj"
        kind "ConsoleApp"
    
    filter "configurations:Release"
        targetdir "build/release/TestApp/bin"
        objdir "build/release/TestApp/obj"
        kind "WindowedApp"
    
    filter {}
