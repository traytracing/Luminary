#pragma once

#include <glm/glm.hpp>
#include "AppStateType.h"

constexpr glm::uvec2 InitialWindowDimensions{ 1920, 1080 };
constexpr int Initialfps = 60;
constexpr glm::vec4 InitialTrailColor{ 0.043f, 0.031f, 0.118f, 1.0f };

struct Settings {
	glm::uvec2 w_Dimensions{ InitialWindowDimensions };
	int fps{ Initialfps };
	AppStateType appState{ AppStateType::Boot };
	
	bool cameraLock{}; // change to checking if in render state instead
	glm::dvec2 MousePosition{}, LMousePosition{}, DMousePosition{};
	bool LeftMouseDown{};


	int renderFrame{ -1 };
	int currentViewObject{ -1 };
	glm::vec4 trailColor{ InitialTrailColor };
	float trailTime = 2.0f;
	float trailWidth = 5.0f;
	float axisWidth = 2.3f;
};