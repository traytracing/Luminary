#ifndef SIMULATION_CLASS_H
#define SIMULATION_CLASS_H

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

class Simulation {
	const GLfloat k = 10.0f;
	float UpdateSim(std::vector<std::pair<std::vector<std::pair<glm::vec4, glm::vec4>>, GLfloat>>& sim, GLfloat deltaTime, GLfloat simTime);

public:
	std::vector<std::pair<std::vector<glm::vec4>, GLfloat>> makeFullSim();
};

#endif