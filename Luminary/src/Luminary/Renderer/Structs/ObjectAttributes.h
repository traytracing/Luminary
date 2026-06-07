#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

struct ObjectAttributes {
	glm::vec2 bNormal;
	float mass;
	glm::vec2 size;
	float seed;
};