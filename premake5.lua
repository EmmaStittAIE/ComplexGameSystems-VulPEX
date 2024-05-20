workspace "Hello Workspace"
    filename "HelloWorkspace"
    startproject "Test Application"

    language "C++"
    cppdialect "C++20"
    toolset "clang"

    location "generated"

    genericBuildPath = ""
    
    configurations { "DebugStatic", "DebugShared", "ReleaseStatic", "ReleaseShared" }
    platforms { "Linux", "Win32", "Win64" }

    filter "configurations:Debug"
        symbols "On"
        optimize "Debug"
    
        defines { "_DEBUG" }

        genericBuildPath = "debug"

    filter "configurations:Release"
        symbols "Off"
        optimize "On"

        defines { "_RELEASE" }

        genericBuildPath = "release"

    filter "platforms:Linux"
        system "linux"
        architecture "x86_64"

        defines { "_LINUX" }

        genericBuildPath = "linux/" .. genericBuildPath

    filter "platforms:Win32"
        system "windows"
        architecture "x86"

        defines { "_WIN32" }

        genericBuildPath = "win32/" .. genericBuildPath

    filter "platforms:Win64"
        system "windows"
        architecture "x86_64"

        defines { "_WIN64" }

        genericBuildPath = "win64/" .. genericBuildPath

    filter {}

project "VulPEX"
    filename "VulPEX"
    targetname "VulPEX"
    targetprefix ""
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
        "src/VulPEX/**.cpp",
    }

    filter "configurations:DebugStatic or ReleaseStatic"
        targetsuffix "-d"
        kind "StaticLib"

    filter "configurations:DebugShared or ReleaseShared"
        kind "SharedLib"

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
        "build/%{cfg.platform}/%{cfg.buildcfg}/VulPEX/include"
    }

    libdirs
    {
        "lib/GLFW",
        libdirs { os.findlib("vulkan") }
    }

    links
    {
        "glfw3",
        "vulkan",
        "VulPEX"
    }

    files
    {
        "src/TestApp/**.cpp",
    }

    filter "configurations:DebugStatic or DebugShared"
        kind "ConsoleApp"

    filter "configurations:ReleaseStatic or ReleaseShared"
        kind "WindowedApp"

    filter {}
