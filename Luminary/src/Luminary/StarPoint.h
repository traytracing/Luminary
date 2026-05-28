#ifndef STAR_POINT_STRUCT_H
#define STAR_POINT_STRUCT_H

#include <random>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Core/PIMath.h"


struct StarPoint {
	glm::vec2 angle;
	GLfloat rotation;
	GLfloat size;
	GLuint starType;
	GLuint starIndex;

	static StarPoint random() {
        static std::mt19937 rng{std::random_device{}()};

        static std::uniform_real_distribution<float> rand(0.0f, 1.0f);
		static std::uniform_int_distribution<unsigned> starTypeRange(0, 1);
		
		static std::uniform_int_distribution<unsigned> starRanges[2] = {
			std::uniform_int_distribution<unsigned>(0, 5), // large star count
			std::uniform_int_distribution<unsigned>(0, 15) // small star count
		};

		float theta = TAU * rand(rng);
		float phi = acosf(1.0f - 2.0f * rand(rng));
		float rot = TAU * rand(rng);
		float s = 0.007f + 0.008f * rand(rng);
		unsigned int type = starTypeRange(rng);
		unsigned int sNum = starRanges[type](rng);

		if (type == 1) s /= 3.0f;

		return { {phi, theta}, rot, s, type, sNum };
	}
};

#endif