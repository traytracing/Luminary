#pragma once

#include <glm/glm.hpp>

static constexpr glm::uvec2 InitialWindowDimensions{ 1920, 1080 };
static constexpr int Initialfps = 60;
static constexpr glm::vec4 InitialTrailColor{ 0.749f, 0.659f, 0.565f, 1.0f };

enum class AppStateType {
	Boot,
	LaunchMenu,
	InEmptyScene,
	InScene,
	Rendering
}; // add simulator screen 

constexpr const char* to_string(AppStateType type) {
	switch (type) {
	case AppStateType::Boot: return "Boot";
	case AppStateType::LaunchMenu: return "LaunchMenu";
	case AppStateType::InEmptyScene: return "InEmptyScene";
	case AppStateType::InScene: return "InScene";
	case AppStateType::Rendering: return "Rendering";
	default: return "Unknown";
	}
}

struct Settings {
	glm::uvec2 w_Dimensions{ InitialWindowDimensions };
	int fps{ Initialfps };
	bool vsync{ true };
	AppStateType appState{ AppStateType::Boot };
	
	bool cameraRenderLock{ false };
	bool cameraGuiLock{ false };
	glm::dvec2 MousePosition{}, LMousePosition{}, DMousePosition{};
	bool LeftMouseDown{};
	float FOVdeg{ 60.0f };
	float nearPlane{ 0.01f };
	float farPlane{ 1000.0f };

	bool screenshotDeposit{ false };
	glm::uvec2 screenshotDimension{};

	int renderFrame{ -1 };
	int maxRenderFrame{ -1 };
	int objectCount{ 0 };
	int objectSource{ -1 };
	bool objectSourceChange{ false };
	glm::vec4 objectSourcePosition{};
	int objectTarget{ -1 };
	bool objectTargetChange{ false };
	glm::vec4 objectTargetPosition{};
	glm::vec4 trailColor{ InitialTrailColor };
	float trailTime = 2.0f;
	float trailWidth = 5.0f;
	float axisWidth = 2.3f;
	
	bool renderGrid{ true };
	bool renderSky{ true };
	bool renderAxes{ true };
	bool renderTrails{ true };
	bool renderObjects{ true };

	int maxDataChunkBytes{ 20000 };
};