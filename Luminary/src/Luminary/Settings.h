#pragma once

#include <glm/glm.hpp>

constexpr glm::uvec2 InitialWindowDimensions{ 1920, 1080 };
constexpr int Initialfps = 60;
constexpr glm::vec4 InitialTrailColor{ 0.043f, 0.031f, 0.118f, 1.0f };

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
	bool cameraGUIClickLock{ false };
	glm::dvec2 MousePosition{}, LMousePosition{}, DMousePosition{};
	bool LeftMouseDown{};


	int renderFrame{ -1 };
	int objectSource{ -1 };
	int objectTarget{ -1 };
	glm::vec4 trailColor{ InitialTrailColor };
	float trailTime = 2.0f;
	float trailWidth = 5.0f;
	float axisWidth = 2.3f;
};