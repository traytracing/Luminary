#include "RenderSystem.h"
#include <stb_image_write.h>
#include <thread>
#include <string>
#include "Luminary/Core/PBO.h"
#include "Platform/OpenGL/imgui_impl_glfw.h"
#include "Platform/OpenGL/imgui_impl_opengl3.h"

GLFWwindow* RenderSystem::MakeWindow() {
	static bool initialized = false;
	if (initialized) {
		throw std::runtime_error("Window already initialized");
	}
	initialized = true;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	GLFWwindow* window = glfwCreateWindow(settings.w_Dimensions.x, settings.w_Dimensions.y, "Canvas", NULL, NULL);
	glfwMakeContextCurrent(window);

	gladLoadGL();
	glViewport(0, 0, settings.w_Dimensions.x, settings.w_Dimensions.y);


	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	const char* glsl_version = "#version 460";

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark();
	return window;
}
RenderSystem::~RenderSystem() {
}
RenderSystem::RenderSystem() {
	static bool initialized = false;
	if (initialized) throw std::runtime_error("RenderSystem already initialized");
	initialized = true;

	Link();

	// REMOVE LATER
	videoManager.UpdateOutputFile("output");
	inputDataProcessor.SetFile(GetAssetPath("inputdata/InputData").c_str());
	inputDataProcessor.AddRenderProgram(nbodyRenderer.objectRenderProgram);
	inputDataProcessor.AddRenderProgram(nbodyRenderer.trailRenderProgram);
	inputDataProcessor.AddRenderProgram(nbodyRenderer.axisRenderProgram);
}

void RenderSystem::Link() {
	std::vector<Camera*> cameras;
	cameras.push_back(&POVCamera);
	cameras.push_back(&SkyCamera);
	inputManager.LinkCameras(cameras);
}

void RenderSystem::Render() {
	skyRenderer.Render();
	nbodyRenderer.Render();
	gridRenderer.Render();
}

#include "imgui.h"

//LOCK EVERYTHING WHEN RENDERING
void RenderSystem::Run() {
	//glfwSwapInterval(0);
	glReadBuffer(GL_BACK);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	
	settings.renderLock = true;
	int frame = 0;
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		inputDataProcessor.UpdatePositionData(frame);


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		static ImVec4 myColor = ImVec4(0.749, 0.659, 0.565, 1.0f);

		ImGui::Begin("Controls");
		//ImGui::Text("This is ImGui running on OpenGL 4.6!");
		float fpsim = ImGui::GetIO().Framerate;
		ImGui::Text("FPS: %.1f", fpsim);
		//ImGui::Text("Seed: %d", randomNum);
		ImGui::SliderInt("View: ", &settings.currentViewObject, 0, 29);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));      // background
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));   // handle
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 0.2f, 0.2f, 1.0f)); // active handle

		//ImGui::SliderInt("FrameIndex: ", &frameIndex, 0, 15599);

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);
		ImGui::ColorEdit4("Pick a color", (float*)&myColor);
		settings.trailColor = glm::vec4(myColor.x, myColor.y, myColor.z, myColor.w);
		static char filePath[128] = "";
		if (ImGui::InputText("File path", filePath, IM_ARRAYSIZE(filePath))) {
		}
		if (ImGui::Button("Load")) {
		}
		//ImGui::Checkbox("Show grid", &renderGrid);

		ImGui::End();
		ImGui::SetNextWindowSize(ImVec2(240, 100)); // width=600, height=200

		ImGui::Begin("Simulation Time", nullptr,
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		//ImGui::Text("%.1f", frameIndex / 165.0f);


		ImGui::End();

		inputManager.Update(window); // BIG


		glViewport(0, 0, settings.w_Dimensions.x, settings.w_Dimensions.y);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.106f, 0.110f, 0.173f, 1.0f);
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Render();

		videoManager.AppendFrame();
	
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
		glfwSwapBuffers(window);
		frame++;
	}
	videoManager.End();

	Cleanup();
}

void RenderSystem::Cleanup() {
	glfwDestroyWindow(window);
}