#include "Gui.h"

#include <algorithm>
#include <stdexcept>
#include <string>

#include <Platform/OpenGL/imgui_impl_glfw.h>
#include <Platform/OpenGL/imgui_impl_opengl3.h>

Gui::Gui(Settings& SRF, GLFWwindow* window)
    : settings(SRF), window(window)
{
    static bool initialized = false;
    if (initialized)
        throw std::runtime_error("Gui already initialized!");

    initialized = true;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.WindowBorderSize = 1.0f;
    style.FramePadding = ImVec2(8.0f, 5.0f);
    style.ItemSpacing = ImVec2(8.0f, 8.0f);

    trailColorGui = ImVec4(
        settings.trailColor.r,
        settings.trailColor.g,
        settings.trailColor.b,
        settings.trailColor.a
    );

    const char* glsl_version = "#version 460";

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

Gui::~Gui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::Update()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (settings.appState == AppStateType::LaunchMenu) {
        DrawLaunchMenu();
        return;
    }

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(
        ImVec2(viewport->WorkPos.x + 16.0f, viewport->WorkPos.y + 16.0f),
        ImGuiCond_FirstUseEver
    );

    ImGui::SetNextWindowSize(
        ImVec2(360.0f, 520.0f),
        ImGuiCond_FirstUseEver
    );

    if (showControls) {
        DrawMainControls();
    }

    if (showSimulation) {
        DrawSimulationPanel();
    }

    if (showRender) {
        DrawRenderPanel();
    }
}

void Gui::Render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::DrawLaunchMenu()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Luminary Launch Menu", nullptr, flags);

    const ImVec2 windowSize = ImGui::GetWindowSize();

    const float menuWidth = 420.0f;
    const float menuHeight = 360.0f;

    ImGui::SetCursorPos(ImVec2(
        (windowSize.x - menuWidth) * 0.5f,
        (windowSize.y - menuHeight) * 0.5f
    ));

    ImGui::BeginChild(
        "LaunchMenuContent",
        ImVec2(menuWidth, menuHeight),
        true,
        ImGuiWindowFlags_NoScrollbar
    );

    ImGui::Spacing();
    ImGui::Spacing();

    const char* title = "LUMINARY";
    const char* subtitle = "Orbital Simulation Renderer";

    float titleWidth = ImGui::CalcTextSize(title).x;
    float subtitleWidth = ImGui::CalcTextSize(subtitle).x;

    ImGui::SetCursorPosX((menuWidth - titleWidth) * 0.5f);
    ImGui::Text("%s", title);

    ImGui::SetCursorPosX((menuWidth - subtitleWidth) * 0.5f);
    ImGui::TextDisabled("%s", subtitle);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint(
        "##LaunchFilePath",
        "example: data/my_file",
        filePathBuffer,
        IM_ARRAYSIZE(filePathBuffer)
    );

    if (ImGui::Button("Load File", ImVec2(-1.0f, 42.0f))) {
        if (OnLoadFile) {
            OnLoadFile(std::string(filePathBuffer));
        }
    }

    ImGui::Spacing();

    if (ImGui::Button("Enter Scene", ImVec2(-1.0f, 42.0f))) {
        settings.appState = AppStateType::InScene;
    }

    if (ImGui::Button("Start Rendering Mode", ImVec2(-1.0f, 42.0f))) {
        settings.appState = AppStateType::Rendering;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Quit", ImVec2(-1.0f, 36.0f))) {
        if (window) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    ImGui::Spacing();

    ImGui::TextDisabled("Tip: load a .lumen scene, then enter the simulation.");

    ImGui::EndChild();

    ImGui::End();
}

void Gui::DrawMainControls()
{
    ImGui::Begin("Luminary Controls", &showControls, ImGuiWindowFlags_NoCollapse);

    const float fps = ImGui::GetIO().Framerate;
    const float frameMs = fps > 0.0f ? 1000.0f / fps : 0.0f;

    ImGui::Text("Performance");
    ImGui::Separator();

    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame time: %.2f ms", frameMs);

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("View", ImGuiTreeNodeFlags_DefaultOpen)) {
        settings.currentViewObject = std::clamp(settings.currentViewObject, 0, maxViewObject);

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::SliderInt("Object", &settings.currentViewObject, 0, maxViewObject);

        ImGui::SameLine();

        if (ImGui::Button("Reset View")) {
            settings.currentViewObject = 0;
        }
    }

    if (ImGui::CollapsingHeader("Trail", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::ColorEdit4("Trail Color", reinterpret_cast<float*>(&trailColorGui));

        settings.trailColor = glm::vec4(
            trailColorGui.x,
            trailColorGui.y,
            trailColorGui.z,
            trailColorGui.w
        );
    }

    if (ImGui::CollapsingHeader("Panels", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Simulation Panel", &showSimulation);
        ImGui::Checkbox("Render Panel", &showRender);
    }

    DrawFilePanel();

    ImGui::End();
}

void Gui::DrawFilePanel()
{
    if (!ImGui::CollapsingHeader("File", ImGuiTreeNodeFlags_DefaultOpen)) {
        return;
    }

    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint(
        "##FilePath",
        "example: data/my_file",
        filePathBuffer,
        IM_ARRAYSIZE(filePathBuffer)
    );

    ImGui::Spacing();

    if (ImGui::Button("Load File", ImVec2(-1.0f, 0.0f))) {
        if (OnLoadFile) {
            OnLoadFile(std::string(filePathBuffer));
        }
    }

    ImGui::TextDisabled("Do not include .lumen unless your loader expects it.");
}

void Gui::DrawSimulationPanel()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(
        ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - 276.0f, viewport->WorkPos.y + 16.0f),
        ImGuiCond_FirstUseEver
    );

    ImGui::SetNextWindowSize(ImVec2(260.0f, 160.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Simulation", &showSimulation, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Current Object: %d", settings.currentViewObject);

    ImGui::Spacing();

    ImGui::Text("Window: %d x %d",
        settings.w_Dimensions.x,
        settings.w_Dimensions.y
    );

    ImGui::Text("Target FPS: %d", settings.fps);

    ImGui::End();
}

void Gui::DrawRenderPanel()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(
        ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - 276.0f, viewport->WorkPos.y + 192.0f),
        ImGuiCond_FirstUseEver
    );

    ImGui::SetNextWindowSize(ImVec2(260.0f, 180.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Render", &showRender, ImGuiWindowFlags_NoCollapse);

    ImGui::Checkbox("Render Lock", &settings.cameraRenderLock);

    ImGui::Spacing();

    if (!recording) {
        if (ImGui::Button("Start Recording", ImVec2(-1.0f, 0.0f))) {
            recording = true;

            if (OnStartRecording) {
                OnStartRecording();
            }
        }
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.15f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));

        if (ImGui::Button("Stop Recording", ImVec2(-1.0f, 0.0f))) {
            recording = false;

            if (OnStopRecording) {
                OnStopRecording();
            }
        }

        ImGui::PopStyleColor(3);
    }

    ImGui::Text("Recording: %s", recording ? "Yes" : "No");

    ImGui::End();
}