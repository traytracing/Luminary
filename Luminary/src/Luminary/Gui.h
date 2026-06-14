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
    std::function<void()> OnStartRecording;
    std::function<void()> OnStopRecording;
    std::function<void()> OnStartRender;
private:
    void DrawLaunchMenuPanel();
    void DrawScenePanel();
    void DrawRenderingPanel();

    void DrawLaunchMenu();
    void DrawSettings();
    void DrawRender();
    void DrawRendering();
private:
    Settings& settings;
    GLFWwindow* window{ nullptr };
    
    //move these to settings
    int maxViewObject{ 29 };
    bool recording{ false };
};