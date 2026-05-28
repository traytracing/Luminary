#ifndef SKY_RENDERER_CLASS_H
#define SKY_RENDERER_CLASS_H

#include <vector>
#include <array>

#include "Luminary/Core/RenderProgram.h"
#include "Luminary/Core/VAO.h"
#include "Luminary/Core/VBO.h"

#include "Luminary/Core/Camera.h"
#include "Luminary/Core/Texture.h"

#include "Luminary/StarPoint.h"

class SkyRenderer {
	friend class RenderSystem;

public: 
	SkyRenderer(const SkyRenderer&) = delete;
	SkyRenderer& operator=(const SkyRenderer&) = delete;
	SkyRenderer(SkyRenderer&&) = delete;
	SkyRenderer& operator=(SkyRenderer&&) = delete;

public:
	SkyRenderer(unsigned int starCount, Camera& camera);
	~SkyRenderer();

	void Render();
	Camera& camera; // automatically updated in rendersystem
private:
	RenderProgram dustRenderProgram{ "SpaceDust.vert", "SpaceDust.frag" };
	RenderProgram starRenderProgram{ "Star.vert", "Star.geom", "Star.frag" };

	VAO dustVAO;
	VAO starVAO;
	VBO starVBO{ GL_STATIC_DRAW };

	Texture largeStarsTex;
	Texture smallStarsTex;
	
	

	unsigned int starCount;
	std::vector<StarPoint> stars;


	void BindTextures();
	std::array<GLuint, 2> starTypeCount;
	std::array<GLint, 2> starSamplers;
	void SetUniforms();
};
#endif