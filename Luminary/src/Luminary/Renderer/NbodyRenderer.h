#ifndef NBODY_RENDERER_CLASS_H
#define NBODY_RENDERER_CLASS_H

#include <vector>
#include <iostream>
#include "Luminary/Core/RenderProgram.h"
#include "Luminary/Core/VAO.h"
#include "Luminary/Core/VBO.h"
#include "Luminary/Core/Camera.h"

#include "Luminary/TrailPoint.h"
#include "Luminary/BlackholePoint.h"



#include "Luminary/Data/Spline3D.h"



// REVIEW BLACKHOLE POINTS AND ACTUAL RENDERING LATER
class NbodyRenderer {
	friend class RenderSystem;
public:
	NbodyRenderer(const NbodyRenderer&) = delete;
	NbodyRenderer& operator=(const NbodyRenderer&) = delete;
	NbodyRenderer(NbodyRenderer&&) = delete;
	NbodyRenderer& operator=(NbodyRenderer&&) = delete;

public:
	NbodyRenderer(Camera& POVCamera);
	~NbodyRenderer();

	
	void Render();

private: // change Render Program shader names later
	RenderProgram blackholeRenderProgram{ "default.vert", "default.geom", "Planet.frag" };
	RenderProgram trailRenderProgram{ "line.vert", "line.frag" };

	VAO blackholeVAO;
	VBO blackholeVBO{ GL_DYNAMIC_DRAW };

	VAO trailVAO;
	VBO trailVBO{ GL_DYNAMIC_DRAW };
	
	Camera& POVCamera;

	std::vector<BlackholePoint> objects;
	std::vector<TrailPoint> lines;

	GLfloat lastTime = 0.0f;

	
	


	std::vector<Spline3D> SplinePathsForObjects;

	void SetUniforms();
};


#endif