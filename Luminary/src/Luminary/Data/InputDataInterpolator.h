#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Luminary/Data/Spline3D.h"

class InputDataInterpolator
{
	
	static void InterpolateData(int timestampsCount, std::vector<double> timestamps, int objectCount, std::vector<std::vector<glm::vec3>> positions);




};