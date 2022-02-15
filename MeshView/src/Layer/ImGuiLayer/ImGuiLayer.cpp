#include"pch.h"
#include "ImGuiLayer.h"

#include<CoreModel/Application.h>
#include<CoreModel/FileDialogs.h>
#include<imgui.h>
#include<backends/imgui_impl_glfw.h>
#include<backends/imgui_impl_opengl3.h>
#include"ImGuizmo.h"
#include<imgui_spectrum.h>
#include<GLFW/glfw3.h>







namespace MV
{
	ImGuiLayer::ImGuiLayer(const std::string& name)
		:Layer(name)
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		//ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
		//ImGui::StyleColorsClassic();
		//ImGui::Spectrum::StyleColorsSpectrum();
		
		SetIniSetting();
		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.

		SetImGuiStyle();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		auto window = (GLFWwindow*)(Application::Get().GetWindow()->GetNativeWindow());
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

	}

	void ImGuiLayer::OnUpdate(TimeStep ts)
	{
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{

		
		//DockSpace Demo End;
		
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::OnImGuiRender()
	{
     

	}

	void ImGuiLayer::OnEvent(Event& e)
	{
	
		if (m_BlockEventFlag&&!e.IsInCategory(EventCategoryAppication))
			e.SetHandled(true);
	}

	void ImGuiLayer::BlockEvent(bool block)
	{
		m_BlockEventFlag = block;
	}

	void ImGuiLayer::SetImGuiStyle()
	{

		//set fonts
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Bold.ttf", 20.0f);
		io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-BoldItalic.ttf", 22.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Regular.ttf", 20.0f);
		

		ImGuiStyle& style = ImGui::GetStyle();
		
		//选择spectrum的风格
		ImGui::Spectrum::StyleColorsSpectrum();
		

		//以下是一些个人修改
		style.WindowBorderSize = 0.0f;
		style.FrameBorderSize = 1.0f;
		style.FrameRounding = 2.0f;
		style.ItemInnerSpacing = { 8.0f,8.0f };
		style.Alpha = 0.9f;
		style.Colors[ImGuiCol_ButtonHovered] = { 0.798f,0.798f,0.798f,1.0f };
		style.Colors[ImGuiCol_Header] = { 0.855f,0.883f,0.913f,1.0f };
		style.Colors[ImGuiCol_HeaderHovered] = { 0.772f,0.828f,0.890f,1.0f };
		style.Colors[ImGuiCol_HeaderActive] = { 0.593f,0.697f,0.821f,1.0f };
		style.Colors[ImGuiCol_Tab] = { 0.855f,0.883f,0.913f,1.0f };
		style.Colors[ImGuiCol_TabHovered] = { 0.772f,0.828f,0.890f,0.8f };
		style.Colors[ImGuiCol_TabActive] = { 0.772f,0.828f,0.890f,0.7f };
		style.Colors[ImGuiCol_TabUnfocused] = style.Colors[ImGuiCol_Tab];
		style.Colors[ImGuiCol_TabUnfocusedActive] = style.Colors[ImGuiCol_TabActive];
		style.Colors[ImGuiCol_PopupBg] = { 1.0f, 1.0f, 1.0f, 0.8f };
		
		style.Colors[ImGuiCol_TitleBg] = style.Colors[ImGuiCol_MenuBarBg];
		style.Colors[ImGuiCol_SliderGrab] = { 0.772f,0.828f,0.890f,1.0f };
		style.Colors[ImGuiCol_SliderGrabActive] = { 0.772f,0.828f,0.890f,1.0f };

	

	}

	void ImGuiLayer::SetIniSetting()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = "./assets/ImguiSetting/imgui.ini";
	}

}
