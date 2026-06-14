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
    Gui(Settings& SRF, GLFWwindow* window);
    ~Gui();

    Gui(const Gui&) = delete;
    Gui& operator=(const Gui&) = delete;

    void Update();
    void Render();

    // Optional hooks you can wire from outside
    std::function<void(const std::string&)> OnLoadFile;
    std::function<void()> OnStartRecording;
    std::function<void()> OnStopRecording;

private:
    void DrawLaunchMenu();

    void DrawScenePanel();
    void DrawRenderingPanel();

    void DrawSettings();
    void DrawRender();
    void DrawRendering();

private:
    Settings& settings;
    GLFWwindow* window{ nullptr };
    
    //move these to settings
    char filePathBuffer[256]{};
    int maxViewObject{ 29 };
    bool recording{ false };
};