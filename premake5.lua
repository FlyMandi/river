---@diagnostic disable: undefined-global, undefined-field
VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace("Cnake")
configurations({ "Debug", "Release" })
location("build")

project("Cnake")
kind("WindowedApp")
language("C++")
cppdialect("C++20")
targetdir("bin/%{cfg.buildcfg}")
includedirs({ "vendor/GLM", "vendor/GLFW/include", "%{VULKAN_SDK}/Include/" })

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
