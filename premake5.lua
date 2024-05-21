workspace "VulPEX Workspace"
    filename "VulPEXWorkspace"
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

project "VulPEX"
    filename "VulPEX"
    targetname "VulPEX"
    targetdir "build/%{cfg.platform}/%{cfg.buildcfg}/VulPEX"
    objdir "build/%{cfg.platform}/%{cfg.buildcfg}/VulPEX/obj"

    prebuildcommands
    {
        "{RMDIR} %[build/%{cfg.platform}/%{cfg.buildcfg}/VulPEX/include]",
        "{COPYDIR} %[src/VulPEX] %[build/%{cfg.platform}/%{cfg.buildcfg}/VulPEX]",
        "{MOVE} %[build/%{cfg.platform}/%{cfg.buildcfg}/VulPEX/VulPEX] %[build/%{cfg.platform}/%{cfg.buildcfg}/VulPEX/include]"
    }

    includedirs
    {
        "lib/glm",
        "lib/GLFW/include",
        "lib/EmmaUtils/include"
    }

    libdirs
    {
        "lib/GLFW",
        "lib/EmmaUtils",
        libdirs { os.findlib("vulkan") }
    }

    links
    {
        "glfw3",
        "EmmaUtils-d",
        "vulkan"
    }

    files
    {
        "src/VulPEX/**.cpp",
    }

    kind "StaticLib"

    filter "configurations:Debug"
        targetsuffix "-d"

    filter {}

project "Test Application"
    filename "TestApp"
    targetname "TestApp"
    targetdir "build/%{cfg.platform}/%{cfg.buildcfg}/TestApp/bin"
    objdir "build/%{cfg.platform}/%{cfg.buildcfg}/TestApp/obj"

    prebuildcommands
    {
        "{RMDIR} %[build/%{cfg.platform}/%{cfg.buildcfg}/TestApp/bin]",
    }

    postbuildcommands
    {
        "{MOVE} %[build/%{cfg.platform}/%{cfg.buildcfg}/TestApp/bin/TestApp] %[working]",
        "{RMDIR} %[build/%{cfg.platform}/%{cfg.buildcfg}/TestApp/bin]",
        "{COPYDIR} %[working] %[build/%{cfg.platform}/%{cfg.buildcfg}/TestApp]",
        "{MOVE} %[build/%{cfg.platform}/%{cfg.buildcfg}/TestApp/working] %[build/%{cfg.platform}/%{cfg.buildcfg}/TestApp/bin]",
        "{DELETE} %[working/TestApp]"
    }

    includedirs
    {
        "lib/glm",
        "lib/GLFW/include",
        "lib/EmmaUtils/include",
        "build/%{cfg.platform}/%{cfg.buildcfg}/VulPEX/include"
    }

    libdirs
    {
        "lib/GLFW",
        "lib/EmmaUtils",
        libdirs { os.findlib("vulkan") }
    }

    links
    {
        "glfw3",
        "EmmaUtils",
        "vulkan",
        "VulPEX"
    }

    files
    {
        "src/TestApp/**.cpp",
    }

    filter "configurations:Debug"
        kind "ConsoleApp"

    filter "configurations:Release"
        kind "WindowedApp"

    filter {}
