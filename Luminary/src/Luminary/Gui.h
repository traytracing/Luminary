#pragma once

#include <string>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>

#include "Settings.h"

class Gui {
public:
    ~Gui();
    Gui(Settings& SRF, GLFWwindow* window);

    void Update();
    void Render();

    // Callback functionss
    std::function<void(const std::string&)> OnLoadFile;
    std::function<void()> OnStartRender;
    std::function<void()> OnScreenshot;
    std::function<void()> DepositScreenshot;
private:
    void DrawLaunchMenuPanel();
    void DrawScenePanel();
    void DrawRenderingPanel();

    void DrawLaunchMenu();
    void DrawSettings();
    void DrawLumenFile();
    void DrawRender();
    void DrawRendering();
private:
    Settings& settings;
    GLFWwindow* window{ nullptr };
};