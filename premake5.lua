---@diagnostic disable: undefined-global
require("premake-ecc/ecc")
workspace("Cnake")
configurations({ "Debug", "Release" })
location("build")

project("Cnake")
kind("WindowedApp")
language("C++")
targetdir("bin/%{cfg.buildcfg}")
libdirs({ "C:/Libraries/" })

files({ "**.h", "**.cpp" })

filter("configurations:Debug")
defines({ "DEBUG" })
symbols("On")

filter("configurations:Release")
defines({ "NDEBUG" })
optimize("On")
