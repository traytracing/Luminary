workspace "Luminary"
	architecture "x64"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Luminary/vendor/GLFW/include"
IncludeDir["Glad"] = "Luminary/vendor/Glad/include"
IncludeDir["ImGui"] = "Luminary/vendor/imgui"
IncludeDir["glm"] = "Luminary/vendor/glm"
IncludeDir["stb_image"] = "Luminary/vendor/stb_image"

include "Luminary/vendor/GLFW"
include "Luminary/vendor/Glad"
include "Luminary/vendor/imgui"

project "Luminary"
	location "Luminary"
	kind "ConsoleApp"
	language "C++"
	buildcustomizations "BuildCustomizations/CUDA 13.2"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.cu",
		"%{prj.name}/vendor/glm/glm",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
	}


	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"$(CUDA_PATH)/include"
	}
	
	libdirs
	{
		"$(CUDA_PATH)/lib/x64"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"cudart"
	}

	filter "system:windows"
		cppdialect "C++20" 
		staticruntime "On"
		systemversion "latest"

		buildoptions { "/utf-8" }

		defines
		{
			"LY_PLATFORM_WINDOWS",
			"GLM_ENABLE_EXPERIMENTAL"
		}

	
	filter "configurations:Debug"
		defines "AU_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "AU_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "AU_DIST"
		buildoptions "/MD"
		optimize "On"