#pragma once

#include <glm/glm.hpp>
#include <iostream>

#include "Luminary/Data/Spline.h"


class Spline3D {
public:
	tk::spline x;
	tk::spline y;
	tk::spline z;

	Spline3D(const std::vector<double>& TimeStamps, const std::vector<glm::vec3>& positions) {
		setPoints(TimeStamps, positions);
	}

	//only cubic interpolation
	void setPoints(const std::vector<double>& TimeStamps, const std::vector<glm::vec3>& positions) {
		std::vector<double> px, py, pz;
		px.reserve(positions.size());
		py.reserve(positions.size());
		pz.reserve(positions.size());

		for (std::size_t i = 0; i < positions.size(); ++i) {
			const auto& p = positions[i];
			if (std::isnan(p.x) || std::isnan(p.y) || std::isnan(p.z)) std::cerr << "Bad Spline Data" << std::endl;

			px.push_back(p.x);
			py.push_back(p.y);
			pz.push_back(p.z);
		}

		x.set_points(TimeStamps, std::move(px), tk::spline::cspline_hermite);
		y.set_points(TimeStamps, std::move(py), tk::spline::cspline_hermite); // cspline
		z.set_points(TimeStamps, std::move(pz), tk::spline::cspline_hermite);
	}

	glm::vec3 evaluate(double t) const { return glm::vec3(x(t), y(t), z(t)); }
};