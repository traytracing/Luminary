#include "Gui.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <Platform/OpenGL/imgui_impl_glfw.h>
#include <Platform/OpenGL/imgui_impl_opengl3.h>

namespace {
    const ImGuiWindowFlags noMoveFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGuiViewport* viewport = nullptr;

    const char* title = "LUMINARY";
    const char* subtitle = "synthesis engine";
    const char* copyrightText = "Copyright symbol 2026 traytracing. All rights reserved.";
    const float launchMenuWidth = 420.0f;
    const float launchMenuHeight = 360.0f;

    const ImVec2 mainPanelSize = ImVec2(420.0f, 840.0f); //420.0f, 640.0f

    char filePathBuffer[256]{};
}
Gui::~Gui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
Gui::Gui(Settings& SRF, GLFWwindow* window) : settings(SRF), window(window) {
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
    style.WindowBorderSize = 1.0f;
    style.FramePadding = ImVec2(8.0f, 5.0f);
    style.ItemSpacing = ImVec2(8.0f, 8.0f);

    const char* glsl_version = "#version 460";

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void Gui::Update() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    viewport = ImGui::GetMainViewport();

    settings.cameraGuiLock = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;

    switch (settings.appState) {
    case AppStateType::LaunchMenu:
        DrawLaunchMenuPanel();
        break;
    case AppStateType::InEmptyScene:
    case AppStateType::InScene:
        DrawScenePanel();
        break;
    case AppStateType::Rendering:
        DrawRenderingPanel();
        break;
    }
}
void Gui::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}




void Gui::DrawLaunchMenuPanel() {
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::Begin("Luminary", nullptr, noMoveFlags);

    DrawLaunchMenu();
 
    ImGui::End();
}
void Gui::DrawScenePanel() {
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(mainPanelSize);
    ImGui::Begin("Scene Panel", 0, noMoveFlags);

    DrawSettings();
    DrawLumenFile();
    DrawRender();
    
    ImGui::End();
}
void Gui::DrawRenderingPanel() {
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(mainPanelSize);
    ImGui::Begin("Render Panel", 0, noMoveFlags);

    DrawRendering();

    ImGui::End();
}

void Gui::DrawLaunchMenu() {
    const ImVec2 windowSize = ImGui::GetWindowSize();
    ImGui::SetCursorPos(ImVec2((windowSize.x - launchMenuWidth) * 0.5f, (windowSize.y - launchMenuHeight) * 0.5f));
    ImGui::BeginChild("LaunchMenuContent", ImVec2(launchMenuWidth, launchMenuHeight), true, ImGuiWindowFlags_NoScrollbar);

    ImGui::Spacing();
    ImGui::Spacing();

    // remove later
    float titleWidth = ImGui::CalcTextSize(title).x;
    float subtitleWidth = ImGui::CalcTextSize(subtitle).x;

    ImGui::SetCursorPosX((launchMenuWidth - titleWidth) * 0.5f);
    ImGui::Text("%s", title);
    ImGui::SetCursorPosX((launchMenuWidth - subtitleWidth) * 0.5f);
    ImGui::TextDisabled("%s", subtitle);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::Text("Source code is available @ github.com/traytracing/Luminary");
    ImGui::Text("for educational purposes only; commerical rights reserved.");
    ImGui::Text("Contact traytracing if you want a specific feature added.");

    ImGui::Spacing();
    ImGui::Text("Note: Don't include .lumen when loading a Lumen file.");

    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::Button("Enter Scene", ImVec2(-1.0f, 42.0f)))
        settings.appState = AppStateType::InEmptyScene;

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    if (ImGui::Button("Quit", ImVec2(-1.0f, 36.0f)))
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    ImGui::Spacing();
    ImGui::SetWindowFontScale(0.75f);
    ImGui::Text(copyrightText);
    ImGui::SetWindowFontScale(1.0f);

    ImGui::EndChild();
}
void Gui::DrawSettings() {
    ImGui::Text("Settings");
    ImGui::Separator();

    ImGui::Text("Window size: %d x %d", static_cast<int>(settings.w_Dimensions.x), static_cast<int>(settings.w_Dimensions.y));

    if (ImGui::Checkbox("VSync", &settings.vsync))
        glfwSwapInterval(settings.vsync ? 1 : 0);
    ImGui::SameLine();
    const float fps = ImGui::GetIO().Framerate;
    ImGui::Text("FPS: %.1f", fps);
}
void Gui::DrawLumenFile() {
    ImGui::Spacing();
    ImGui::Text("Lumen File");
    ImGui::Separator();
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##FilePath", "File name (ex. InputData)", filePathBuffer, IM_ARRAYSIZE(filePathBuffer));
    if (ImGui::Button("Load File", ImVec2(-1.0f, 0.0f)))
        OnLoadFile(std::string(filePathBuffer));
}
void Gui::DrawRender() {
    ImGui::Spacing();
    ImGui::Text("Render");
    ImGui::Separator();

    ImGui::InputInt("Max Chunk Bytes", &settings.maxDataChunkBytes, 20000, 10000000);

    ImGui::InputInt("Render FPS ##input", &settings.fps);
    settings.fps = std::clamp(settings.fps, 10, 200);

    ImGui::Checkbox("Camera Lock", &settings.cameraRenderLock);

    ImGui::SliderInt("Object Source (from)", &settings.objectSource, -1, settings.objectCount-1);
    settings.objectSource = std::clamp(settings.objectSource, -1, settings.objectCount-1);
    ImGui::SliderInt("Object Target (to)", &settings.objectTarget, -1, settings.objectCount-1);
    settings.objectTarget = std::clamp(settings.objectTarget, -1, settings.objectCount-1);

    ImGui::SliderFloat("FOV degree", &settings.FOVdeg, 30.0f, 110.0f);
    ImGui::SliderFloat("Near Plane", &settings.nearPlane, 0.01f, 0.1f);
    ImGui::SliderFloat("Far Plane", &settings.farPlane, 10.0f, 10000.0f);

    ImGui::Checkbox("Render Grid", &settings.renderGrid);
    ImGui::Checkbox("Render Sky", &settings.renderSky);
    ImGui::Checkbox("Render Objects", &settings.renderObjects);
    ImGui::Checkbox("Render Trails", &settings.renderTrails);
    ImGui::ColorEdit4("Trail Color", reinterpret_cast<float*>(&settings.trailColor));
    ImGui::SliderFloat("Trail Time", &settings.trailTime, 0.001f, 20.0f);
    ImGui::SliderFloat("Trail Width", &settings.trailWidth, 0.001f, 10.0f);
    ImGui::Checkbox("Render Axes", &settings.renderAxes);
    ImGui::SliderFloat("Axis Width", &settings.axisWidth, 0.001f, 10.0f);

    if (settings.appState == AppStateType::InScene) {
        if (ImGui::Button("Start Render"))
            OnStartRender();

        ImGui::SliderInt("Render Frame", &settings.renderFrame, -1, settings.maxRenderFrame);
        if (ImGui::Button("Screenshot"))
            OnScreenshot();
    }
}
void Gui::DrawRendering() {
    //fix 
    ImGui::Spacing();
    ImGui::Text("Rendering");
    ImGui::Separator();


    ImGui::SetNextItemWidth(-1.0f);

    ImGui::SameLine();

    if (ImGui::Button("Reset View")) {
        settings.objectSource = 0;
    }

    ImGui::Checkbox("Camera Lock", &settings.cameraRenderLock);
    ImGui::Spacing();
    ImGui::Text("Current Object: %d", settings.objectSource);
    ImGui::Spacing();
    ImGui::Text("Window: %d x %d", settings.w_Dimensions.x, settings.w_Dimensions.y);
}