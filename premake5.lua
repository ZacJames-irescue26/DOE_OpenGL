workspace "Decay_Of_Environment"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"

	}
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
project "Decay_Of_Environment"
	location "Decay_Of_Environment"
	kind "ConsoleApp"
	laguage "c++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h"
		"%{prj.name}/src/**.cpp"
	}

	include
	{

	}