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
	gui.OnStartRender = [&]() {
		if (settings.appState == AppStateType::InScene && inputDataProcessor.processorState == ProcessorState::FileLoaded)
			settings.appState = AppStateType::Rendering;
	};
	gui.DepositScreenshot = [&]() {
		if (!settings.screenshotDeposit)
			return;

		glBindBuffer(GL_PIXEL_PACK_BUFFER, videoManager.screenshotPBO.GetID());
		glBufferData(GL_PIXEL_PACK_BUFFER, settings.screenshotDimension.x * settings.screenshotDimension.y * 3, NULL, GL_STREAM_READ);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadBuffer(GL_BACK);
		glReadPixels(0, 0, settings.screenshotDimension.x, settings.screenshotDimension.y, GL_RGB, GL_UNSIGNED_BYTE, 0);

		unsigned char* gpuPtr = (unsigned char*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		if (gpuPtr) {
			stbi_flip_vertically_on_write(true);
			static int screenshotID = 0;
			std::string screenshotName = GetAssetPath("outputs/screenshot" + std::to_string(screenshotID) + ".png");
			stbi_write_png(screenshotName.c_str(), settings.screenshotDimension.x, settings.screenshotDimension.y, 3, gpuPtr, settings.screenshotDimension.x * 3);
			screenshotID++;
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		}
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		settings.screenshotDeposit = false;
	};
	gui.OnScreenshot = [&]() {
		settings.screenshotDimension = settings.w_Dimensions;
		settings.screenshotDeposit = true;
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

	if (settings.renderSky) skyRenderer.Render();
	if (settings.renderGrid) gridRenderer.Render();

	gui.Render();
}

void RenderSystem::InSceneLoop() {
	inputDataProcessor.UpdateData(settings.renderFrame);

	glViewport(0, 0, settings.w_Dimensions.x, settings.w_Dimensions.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.106f, 0.110f, 0.173f, 1.0f);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (settings.renderSky) skyRenderer.Render();
	nbodyRenderer.Render();
	if (settings.renderGrid) gridRenderer.Render();

	gui.DepositScreenshot();
	gui.Render();
}

void RenderSystem::RenderingLoop() {
	inputDataProcessor.UpdateData(settings.renderFrame);

	glViewport(0, 0, settings.w_Dimensions.x, settings.w_Dimensions.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.106f, 0.110f, 0.173f, 1.0f);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (settings.renderSky) skyRenderer.Render();
	nbodyRenderer.Render();
	if (settings.renderGrid) gridRenderer.Render();

	
	static bool startVideo = true;
	if (startVideo) {
		videoManager.UpdateOutputFile("output");
		videoManager.Start();
		startVideo = false;
	}

	videoManager.AppendFrame();
	gui.DepositScreenshot();
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