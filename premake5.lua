workspace("Cnake")
configurations({ "Debug", "Release" })

project("Cnake")
kind("WindowedApp")
language("C++")
targetdir("bin/%{cfg.buildcfg}")

files({ "**.h", "**.cpp" })

filter("configurations:Debug")
defines({ "DEBUG" })
symbols("On")

filter("configurations:Release")
defines({ "NDEBUG" })
optimize("On")
