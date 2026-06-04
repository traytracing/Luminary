#pragma once

#include <glm/glm.hpp>

constexpr glm::uvec2 InitialWindowDimensions{ 1920, 1080 };
constexpr int Initialfps = 60;

struct Settings {
	glm::uvec2 w_Dimensions{ InitialWindowDimensions };
	int fps{ Initialfps };
};