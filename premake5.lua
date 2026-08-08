---@diagnostic disable: undefined-global, undefined-field
VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace("river3D")
    configurations({"debug", "asan", "release"})
    platforms({"linux", "windows"})
    location("build")
    architecture("x86_64")

project("river3D_common")
    language("C")
    cdialect("C99")
    warnings("Extra")
    kind("StaticLib")
    targetname("river3Dcommon")
    libdirs({"./vendor/imgsurf/bin/%{cfg.buildcfg}/"})
    includedirs({"./include/",
                 "/usr/include/",
                 "./vendor/puddle/include/",
                 "./vendor/imgsurf/include/"})
    buildoptions({"-Wextra", "-Wall", "-Wpedantic", "-Wconversion", "-Wshadow",
                  "-Wsign-compare", "Wtype-limits"})
    links("imgsurf:static")

    filter("configurations:asan")
        defines{"ASAN"}

    filter("configurations:debug")
        defines{"DEBUG"}

    filter("configurations:debug or asan")
        runtime("debug")
        symbols("On")
        optimize("Off")

    filter("configurations:release")
        staticruntime("off")
        runtime("release")
        symbols("Off")
        optimize("Speed")

    filter("platforms:Linux")
        system("Linux")
        defines("BUILD_LINUX")
        targetdir("bin/%{cfg.buildcfg}")
        objdir("obj/river3Dcommon/")
        files({"./src/river3D_*",
               "./include/river3D_*",
               "./src/linux_river3Dcommon*",
               "./include/linux_river3Dcommon*",
               "./src/river3Dcommon*",
               "./include/river3Dcommon*"})
        linkoptions({"-lX11", "-fuse-ld=mold"})
        toolset("clang")

    filter("platforms:Windows")
        system("Windows")
        defines("BUILD_WINDOWS")
        targetdir("bin/%{cfg.buildcfg}")
        objdir("obj/")
        files({"./src/river3D_*",
               "./include/river3D_*",
               "./src/win32_river3Dcommon*",
               "./include/win32_river3Dcommon*",
               "./src/river3Dcommon*",
               "./include/river3Dcommon*" })

    filter({"platforms:Linux", "configurations:debug or asan"})
        buildoptions({"-gfull", "-O1"})
        linkoptions({"-gfull", "-O1"})

    filter({"platforms:Linux", "configurations:asan"})
        buildoptions({"-fsanitize=address,leak,undefined", "-fno-omit-frame-pointer",
                      "-static-libasan"})
        linkoptions({"-fsanitize=address,leak,undefined", "-fno-omit-frame-pointer",
                     "-static-libasan"})

    filter({"platforms:Windows", "configurations:asan"})
        editandcontinue("Off")
        buildoptions({"/fsanitize=address", "/Zi", "/INCREMENTAL:NO"})

project("river3D_software")
    language("C")
    cdialect("C99")
    warnings("Extra")
    kind("SharedLib")
    targetname("river3Dsoftware")
    libdirs({"./vendor/imgsurf/bin/%{cfg.buildcfg}/", "./bin/%{cfg.buildcfg}/"})
    includedirs({"./include/",
                 "/usr/include/",
                 "./vendor/puddle/include/",
                 "./vendor/imgsurf/include/"})
    buildoptions({"-Wextra", "-Wall", "-Wpedantic", "-Wconversion", "-Wshadow",
                  "-Wsign-compare", "Wtype-limits"})
    links({"imgsurf:static", "river3Dcommon:static"})

    filter("configurations:asan")
        defines{"ASAN"}

    filter("configurations:debug")
        defines{"DEBUG"}

    filter("configurations:debug or asan")
        runtime("debug")
        symbols("On")
        optimize("Off")

    filter("configurations:release")
        staticruntime("off")
        runtime("release")
        symbols("Off")
        optimize("Speed")

    filter("platforms:Linux")
        system("Linux")
        defines("BUILD_LINUX")
        targetdir("bin/%{cfg.buildcfg}")
        objdir("obj/river3Dsoftware/")
        files({"./src/linux_river3Dsoftware*",
               "./include/linux_river3Dsoftware*",
               "./src/river3Dsoftware*",
               "./include/river3Dsoftware*" })
        linkoptions({"-lX11", "-lXrender", "-lriver3Dcommon", "-lm", "-fuse-ld=mold"})
        toolset("clang")

    filter("platforms:Windows")
        system("Windows")
        defines("BUILD_WINDOWS")
        targetdir("bin/%{cfg.buildcfg}")
        objdir("obj/")
        files({"./src/win32_river3Dsoftware*",
               "./include/win32_river3Dsoftware*",
               "./src/river3Dsoftware*",
               "./include/river3Dsoftware*" })
        links({"imgsurf.lib", "river3Dcommon.lib"})
        buildoptions({"/wd4068"})

    filter({"platforms:Linux", "configurations:debug or asan"})
        buildoptions({"-gfull", "-O1"})
        linkoptions({"-gfull", "-O1"})

    filter({"platforms:Linux", "configurations:asan"})
        buildoptions({"-fsanitize=address,leak,undefined", "-fno-omit-frame-pointer",
                      "-static-libasan"})
        linkoptions({"-fsanitize=address,leak,undefined", "-fno-omit-frame-pointer",
                     "-static-libasan"})

    filter({"platforms:Windows", "configurations:asan"})
        editandcontinue("Off")
        buildoptions({"/fsanitize=address", "/Zi", "/INCREMENTAL:NO"})

    filter({"platforms:Windows", "configurations:release"})
        linkoptions("/NODEFAULTLIB:MSVCRTD")

project("river3D_vulkan")
    language("C")
    cdialect("C99")
    warnings("Extra")
    kind("SharedLib")
    targetname("river3Dvulkan")
    libdirs({"./vendor/imgsurf/bin/%{cfg.buildcfg}/", "./bin/%{cfg.buildcfg}/"})
    includedirs({"./include/",
                 "/usr/include/",
                 "./vendor/puddle/include/",
                 "./vendor/imgsurf/include/"})
    buildoptions({"-Wextra", "-Wall", "-Wpedantic", "-Wconversion", "-Wshadow",
                  "-Wsign-compare", "Wtype-limits"})
    links({"imgsurf:static", "river3Dcommon:static"})

    filter("configurations:asan")
        defines{"ASAN"}

    filter("configurations:debug")
        defines{"DEBUG"}

    filter("configurations:debug or asan")
        runtime("debug")
        symbols("On")
        optimize("Off")

    filter("configurations:release")
        staticruntime("off")
        runtime("release")
        symbols("Off")
        optimize("Speed")

    filter("platforms:Linux")
        system("Linux")
        defines("BUILD_LINUX")
        targetdir("bin/%{cfg.buildcfg}")
        objdir("obj/river3Dvulkan/")
        files({"./src/linux_river3Dvulkan*",
               "./include/linux_river3Dvulkan*",
               "./src/river3Dvulkan*",
               "./include/river3Dvulkan*" })
        linkoptions({"-lX11", "-lXrender", "-lriver3Dcommon", "-lm", "-fuse-ld=mold"})
        toolset("clang")

    filter("platforms:Windows")
        system("Windows")
        defines("BUILD_WINDOWS")
        targetdir("bin/%{cfg.buildcfg}")
        objdir("obj/")
        files({"./src/win32_river3Dvulkan*",
               "./include/win32_river3Dvulkan*",
               "./src/river3Dvulkan*",
               "./include/river3Dvulkan*" })

    filter({"platforms:Linux", "configurations:debug or asan"})
        buildoptions({"-gfull", "-O1"})
        linkoptions({"-gfull", "-O1"})

    filter({"platforms:Linux", "configurations:asan"})
        buildoptions({"-fsanitize=address,leak,undefined", "-fno-omit-frame-pointer",
                      "-static-libasan"})
        linkoptions({"-fsanitize=address,leak,undefined", "-fno-omit-frame-pointer",
                     "-static-libasan"})

    filter({"platforms:Windows", "configurations:asan"})
        editandcontinue("Off")
        buildoptions({"/fsanitize=address", "/Zi", "/INCREMENTAL:NO"})

    filter({"platforms:Windows", "configurations:release"})
        linkoptions("/NODEFAULTLIB:MSVCRTD")
