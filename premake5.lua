---@diagnostic disable: undefined-global
workspace("Cnake")
configurations({ "Debug", "Release" })
location("build")

project("Cnake")
kind("WindowedApp")
language("C++")
targetdir("bin/%{cfg.buildcfg}")
includedirs({ "../Libraries/GLM", "../Libraries/GLFW/include", "../../VulkanSDK/1.3.296.0/Include" })

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
		print("done.")
	end,
})
