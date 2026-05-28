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

    GLfloat zoom = 1.0f; // dont need
    GLdouble lastMouseX = 0.0, lastMouseY = 0.0;  // dont need
    GLdouble panOffsetX = 0.0, panOffsetY = 0.0; // dont need

    GLboolean isPanning;  // dont need

    void UpdateCameras(GLFWwindow* window);
};
#endif