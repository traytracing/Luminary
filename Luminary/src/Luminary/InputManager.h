#ifndef INPUT_MANAGER_CLASS_H
#define INPUT_MANAGER_CLASS_H

#include <vector>

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
    InputManager(Camera& POVCamera, Camera& SkyCamera);

    void Update(GLFWwindow* window);

    static void window_size_callback(GLFWwindow* window, int width, int height);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
    Camera& POVCamera; 
    Camera& SkyCamera;

    void UpdateCameras(GLFWwindow* window);
};
#endif