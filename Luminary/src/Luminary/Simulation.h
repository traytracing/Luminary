#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <string>
#include <vector>

class Simulation {
public:
	enum class SimType {
		Original,
		Spiral
	};
public:
	Simulation() = default;
	~Simulation() = default;

	void MakeSim(const std::string& filename, const SimType& type);
private:
	bool WriteSimToFile(const std::string& filename, const std::vector<std::pair<std::vector<glm::vec4>, GLfloat>>& OutSim);

	std::vector<std::pair<std::vector<glm::vec4>, GLfloat>> MakeOriginalSim();
	std::vector<std::pair<std::vector<glm::vec4>, GLfloat>> MakeSpiralSim();
};