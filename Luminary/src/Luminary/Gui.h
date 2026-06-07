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

    void DrawMainControls();
    void DrawSimulationPanel();
    void DrawRenderPanel();
    void DrawFilePanel();

private:
    Settings& settings;
    GLFWwindow* window{ nullptr };

    bool showControls{ true };
    bool showSimulation{ true };
    bool showRender{ true };

    char filePathBuffer[256]{};

    ImVec4 trailColorGui{ 0.749f, 0.659f, 0.565f, 1.0f };

    int maxViewObject{ 29 };

    bool recording{ false };
};