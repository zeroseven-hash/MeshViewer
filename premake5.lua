workspace "MeshView"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

    startproject "MeshView"

flags {"MultiProcessorCompile"}
outputdir="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir={}
IncludeDir["GLFW"]="MeshView/extern/GLFW/include"
IncludeDir["GLAD"]="MeshView/extern/GLAD/include"
IncludeDir["ImGui"]="MeshView/extern/ImGui"
IncludeDir["GLM"]="MeshView/extern/glm"
IncludeDir["Entt"]="MeshView/extern/entt/include"
IncludeDir["ImGuizmo"]="MeshView/extern/ImGuizmo"
group "Dependencies"
	include "MeshView/extern/GLFW"
    include "MeshView/extern/GLAD"
    include "MeshView/extern/ImGui"
group ""


project "MeshView"
    location "MeshView"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
	pchsource "MeshView/pch.cpp"

    files
	{
        "%{prj.name}/*.cpp",
        "%{prj.name}/*.h",
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
        "%{prj.name}/extern/ImGuizmo/ImGuizmo.h",
        "%{prj.name}/extern/ImGuizmo/ImGuizmo.cpp"
	}


    includedirs
    {

        "%{prj.name}/",
        "%{prj.name}/src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.Entt}",
        "%{IncludeDir.ImGuizmo}",
        "%{prj.name}/extern/spdlog/include",
        "%{prj.name}/extern/stb_image"
    }

    links
    {
        "GLFW",
        "GLAD",
        "ImGui",
    }

    filter "files:MeshView/extern/ImGuizmo/**.cpp"
	flags{"NoPCH"}

    filter "system:windows"
        systemversion "latest"

        defines
        {
            
        }

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "RELEASE"
        runtime "Release"
        optimize "On"

        
