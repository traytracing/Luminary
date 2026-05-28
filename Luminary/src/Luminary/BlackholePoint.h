#ifndef BLACKHOLE_POINT_STRUCT_H
#define BLACKHOLE_POINT_STRUCT_H

#include <glm/glm.hpp>
#include <glad/glad.h>

struct BlackholePoint {
	glm::vec3 position;
	glm::vec2 bNormal;
	GLfloat mass;
	glm::vec2 size;
	GLfloat seed;
};

#endif