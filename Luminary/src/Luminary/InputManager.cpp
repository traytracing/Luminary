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

void InputManager::Update(GLFWwindow* window) {
	settings.LMousePosition = settings.MousePosition;
	glfwGetCursorPos(window, &settings.MousePosition.x, &settings.MousePosition.y);
	settings.MousePosition.y = (-settings.MousePosition.y + settings.w_Dimensions.y);
	settings.DMousePosition = settings.MousePosition - settings.LMousePosition;

	for (auto& func : updateFunctions) func();
}

void InputManager::LinkCameras(std::vector<Camera*> cameras) {
	static bool initialized = false;
	initialized ? throw std::runtime_error("Cameras already linked!") : initialized = true;

	Camera* povCamera = cameras[0];
	auto a0 = [this, povCamera] {
		if (!settings.cameraLock)
			povCamera->Inputs(window);

		povCamera->UpdateMatrix(45.0f, 0.1f, 1000.0f);
	};
	updateFunctions.push_back(std::move(a0));

	Camera* skyCamera = cameras[1];
	auto a1 = [this, povCamera, skyCamera] {
		skyCamera->orientation = povCamera->orientation;
		skyCamera->UpdateMatrix(45.0f, 0.1f, 1000.0f);
	};
	updateFunctions.push_back(std::move(a1));
}

void InputManager::window_size_callback(GLFWwindow* window, int width, int height) {
	InputManager* self = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
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