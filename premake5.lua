---@diagnostic disable: undefined-global
require("ecc/ecc")
workspace("Cnake")
configurations({ "Debug", "Release" })
location("build")

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

newaction({
	trigger = "clean",
	description = "clean the software",
	execute = function()
		print("clean the build...")
		os.rmdir("./build/")
		os.remove("compile_commands.json")
		os.rmdir("./.cache/")
		print("done.")
	end,
})
