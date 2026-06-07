#pragma once

#include <vector>

#include "Luminary/Settings.h"
#include "Core/Camera.h"

class InputManager {
    friend class RenderSystem;
public:
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    InputManager(InputManager&&) = delete;
    InputManager& operator=(InputManager&&) = delete;

public:
    ~InputManager();
    InputManager(Settings& settings, GLFWwindow* window);

    void Update();

    static void window_size_callback(GLFWwindow* window, int width, int height);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
public:
    void LinkCameras(std::vector<Camera*> cameras);
private:
    Settings& settings;
    GLFWwindow* window;

    std::vector<std::function<void()>> updateFunctions;
};