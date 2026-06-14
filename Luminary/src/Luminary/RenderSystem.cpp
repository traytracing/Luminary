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

	return window;
}
RenderSystem::~RenderSystem() {
}
RenderSystem::RenderSystem() {
	static bool initialized = false;
	if (initialized) throw std::runtime_error("RenderSystem already initialized");
	initialized = true;

	Link();
	
}
void RenderSystem::Link() {
	std::vector<Camera*> cameras;
	cameras.push_back(&POVCamera);
	cameras.push_back(&SkyCamera);
	inputManager.LinkCameras(cameras);

	// FIX 
	gui.OnLoadFile = [&](const std::string& path) {
		inputDataProcessor.SetFile(path);
		settings.appState = inputDataProcessor.processorState == ProcessorState::FileLoaded ? AppStateType::InScene : AppStateType::InEmptyScene;
	};
	gui.OnStartRecording = [&]() {
		videoManager.UpdateOutputFile("output");
	};
	gui.OnStopRecording = [&]() {
		videoManager.End();
	};
	
	inputDataProcessor.AddRenderProgram(nbodyRenderer.objectRenderProgram);
	inputDataProcessor.AddRenderProgram(nbodyRenderer.trailRenderProgram);
	inputDataProcessor.AddRenderProgram(nbodyRenderer.axisRenderProgram);
}


void RenderSystem::Run() {
	settings.appState = AppStateType::LaunchMenu;
	Loop();
	Cleanup();
}

void RenderSystem::Loop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		gui.Update();
		inputManager.Update();

		switch (settings.appState) {
		case AppStateType::LaunchMenu:
			LaunchMenuLoop();
			break;
		case AppStateType::InEmptyScene:
			InEmptySceneLoop();
			break;
		case AppStateType::InScene:
			InSceneLoop();
			break;
		case AppStateType::Rendering:
			RenderingLoop();
			break;
		}
		glfwSwapBuffers(window);
	}
}


void RenderSystem::LaunchMenuLoop() {
	glViewport(0, 0, settings.w_Dimensions.x, settings.w_Dimensions.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.106f, 0.110f, 0.173f, 1.0f);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gui.Render();
}

void RenderSystem::InEmptySceneLoop() {
	glViewport(0, 0, settings.w_Dimensions.x, settings.w_Dimensions.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.106f, 0.110f, 0.173f, 1.0f);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skyRenderer.Render();
	gridRenderer.Render();

	gui.Render();
}

void RenderSystem::InSceneLoop() {
	inputDataProcessor.UpdateData(settings.renderFrame);

	glViewport(0, 0, settings.w_Dimensions.x, settings.w_Dimensions.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.106f, 0.110f, 0.173f, 1.0f);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skyRenderer.Render();
	nbodyRenderer.Render();
	gridRenderer.Render();

	gui.Render();
}

void RenderSystem::RenderingLoop() {
	inputDataProcessor.UpdateData(settings.renderFrame);

	glViewport(0, 0, settings.w_Dimensions.x, settings.w_Dimensions.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.106f, 0.110f, 0.173f, 1.0f);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skyRenderer.Render();
	nbodyRenderer.Render();
	gridRenderer.Render();

	
	static bool startVideo = true;
	if (startVideo) {
		videoManager.UpdateOutputFile("output");
		videoManager.Start();
		startVideo = false;
	}

	videoManager.AppendFrame();
	gui.Render();

	if (inputDataProcessor.AtLastRenderFrame()) {
		settings.appState = AppStateType::InScene;
		startVideo = true;
		videoManager.End();
	}
}

void RenderSystem::Cleanup() {
	videoManager.End();
	glfwDestroyWindow(window);
}