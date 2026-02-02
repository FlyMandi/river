---@diagnostic disable: undefined-global, undefined-field
require"vendor/premake-ecc/ecc"
VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace("River")
    configurations({ "Debug", "Release" })
    platforms({"Win64"})
    location("build")

    project("River")
        language("C++")
        cppdialect("C++23")
        targetdir("bin/%{cfg.platform}_%{cfg.buildcfg}")
        objdir("obj/%{cfg.platform}_%{cfg.buildcfg}")
        includedirs({ "./include/", "./vendor/glfw-3.4-win64/include/", "%{VULKAN_SDK}/Include/" })
        syslibdirs({ "%{VULKAN_SDK}/Lib/", "./vendor/glfw-3.4-win64/lib-vc2022/" })
        files({ "**.h", "**.c", "**.hpp", "**.cpp" })

filter("configurations:Debug")
    kind("ConsoleApp")
    defines{"DEBUG"}
    staticruntime("off")
    runtime("Debug")
    symbols("On")
    ignoredefaultlibraries({ "MSVCRT" })

filter("configurations:Release")
    kind("WindowedApp")
    staticruntime("off")
    runtime("Release")
    symbols("Off")
    optimize("Speed")

filter("platforms:Win64")
    links{ "glfw3", "vulkan-1" }
    system("Windows")
    architecture("x86_64")

-- filter("platforms:Unix")
    -- links{ "glfw3", "vulkan" }
--     system("linux")
--     architecture("x86_64")

newaction({
    trigger = "clean",
    description = "clean the software",
    execute = function()
        print("clean the build...")
        os.rmdir("./build/")
        os.rmdir("./bin/")
        os.rmdir("./obj/")
        os.rmdir("./.cache/")
        os.remove("BuildRules.xml")
        os.remove("BuildRules.props")
        os.remove("BuildRules.targets")
        print("done.")
    end,
})
