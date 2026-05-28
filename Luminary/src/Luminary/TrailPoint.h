#ifndef TRAIL_POINT_STRUCT_H
#define TRAIL_POINT_STRUCT_H

#include <glm/glm.hpp>
#include <glad/glad.h>

struct TrailPoint {
	glm::vec3 bpos;
	GLfloat renderbool;
	glm::vec4 color;
};

#endif