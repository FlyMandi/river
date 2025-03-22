---@diagnostic disable: undefined-global, undefined-field
VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace("Cnake")
    configurations({ "Debug", "Release" })
    platforms({ "Win64" })
    location("build")

    project("River")
        kind("StaticLib")
        links{ "glfw3", "vulkan-1" }
        language("C++")
        cppdialect("C++20")
        targetdir("bin/%{cfg.buildcfg}")
        includedirs({ "./include/", "./vendor/glfw-3.4-win64/include/", "%{VULKAN_SDK}/Include/" })
        syslibdirs({ "%{VULKAN_SDK}/Lib/", "./vendor/glfw-3.4-win64/lib-vc2022/" })
        files({ "**.h", "**.c", "**.hpp", "**.cpp" })

filter("configurations:Debug")
    defines{"DEBUG"}
    staticruntime("off")
    runtime("Debug")
    symbols("On")
    ignoredefaultlibraries({ "MSVCRT" })

filter("configurations:Release")
    staticruntime("off")
    runtime("Release")
    symbols("Off")
    optimize("Speed")

filter("platforms:Win64")
    system("Windows")
    architecture("x86_64")

newaction({
    trigger = "clean",
    description = "clean the software",
    execute = function()
        print("clean the build...")
        os.rmdir("./build/")
        os.rmdir("./bin/")
        os.remove("BuildRules.xml")
        os.remove("BuildRules.props")
        os.remove("BuildRules.targets")
        print("done.")
    end,
})
