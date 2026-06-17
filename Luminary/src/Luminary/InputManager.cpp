#include "InputManager.h"
#include <iostream>
#include <filesystem>

InputManager::~InputManager() {
}
InputManager::InputManager(Settings& settings, GLFWwindow* window) : settings(settings), window(window) {
	static bool initialized = false;
	if (initialized) throw std::runtime_error("InputManager already initialized");
	initialized = true;

	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetWindowUserPointer(window, this);
}

void InputManager::window_size_callback(GLFWwindow* window, int width, int height) {
	InputManager* self = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
	if (self->settings.appState == AppStateType::Rendering) {
		glm::uvec2 lockedSize = self->settings.w_Dimensions;
		glfwSetWindowSize(window, lockedSize.x, lockedSize.y);
	}
	else {
		self->settings.w_Dimensions = glm::uvec2(width, height);
	}
}
void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	InputManager* self = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
}
void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	InputManager* self = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		self->settings.LeftMouseDown = GL_TRUE;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		self->settings.LeftMouseDown = GL_FALSE;
	}
}

void InputManager::Update() {
	if (settings.appState == AppStateType::InEmptyScene)
		settings.renderFrame = -1;
	if (settings.appState == AppStateType::InScene)
		settings.renderFrame = std::clamp(settings.renderFrame, -1, settings.maxRenderFrame);
	if (settings.appState == AppStateType::Rendering)
		settings.renderFrame++;

	settings.LMousePosition = settings.MousePosition;
	glfwGetCursorPos(window, &settings.MousePosition.x, &settings.MousePosition.y);
	settings.MousePosition.y = -settings.MousePosition.y + settings.w_Dimensions.y;
	settings.DMousePosition = settings.MousePosition - settings.LMousePosition;

	for (auto& func : updateFunctions) func();
}

void InputManager::LinkCameras(std::vector<Camera*> cameras) {
	static bool initialized = false;
	initialized ? throw std::runtime_error("Cameras already linked!") : initialized = true;

	Camera* povCamera = cameras[0];
	auto a0 = [this, povCamera] {
		if (settings.appState != AppStateType::InEmptyScene && settings.appState != AppStateType::InScene && settings.appState != AppStateType::Rendering)
			return;
		if (!settings.cameraRenderLock && !settings.cameraGuiLock)
			povCamera->Inputs(window);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		povCamera->UpdateMatrix(settings.FOVdeg, settings.nearPlane, settings.farPlane);
	};
	updateFunctions.push_back(std::move(a0));

	Camera* skyCamera = cameras[1];
	auto a1 = [this, povCamera, skyCamera] {
		if (settings.appState != AppStateType::InEmptyScene && settings.appState != AppStateType::InScene && settings.appState != AppStateType::Rendering)
			return;
		if (!settings.cameraRenderLock && !settings.cameraGuiLock)
			skyCamera->orientation = povCamera->orientation;
		skyCamera->UpdateMatrix(settings.FOVdeg, settings.nearPlane, settings.farPlane);
	};
	updateFunctions.push_back(std::move(a1));
}