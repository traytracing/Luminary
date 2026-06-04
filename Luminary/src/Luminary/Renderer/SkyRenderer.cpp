#include "SkyRenderer.h"

// FINISH THIS LATER
SkyRenderer::~SkyRenderer() {

}

SkyRenderer::SkyRenderer(unsigned int starCount, Camera& camera)
	: starCount(starCount), camera(camera) {
	static bool initialized = false;
	if (initialized) throw std::runtime_error("SkyRender already initialized");
	initialized = true;

	stars.reserve(starCount);
	for (int i = 0; i < starCount; ++i) {
		stars.emplace_back(StarPoint::random());
	}
	starVBO.SetDataOnce(stars.data(), stars.size() * sizeof(StarPoint));

	starVAO.LinkAttrib(starVBO, 0, 2, GL_FLOAT, sizeof(StarPoint), (void*) 0);
	starVAO.LinkAttrib(starVBO, 1, 1, GL_FLOAT, sizeof(StarPoint), (void*) offsetof(StarPoint, rotation));
	starVAO.LinkAttrib(starVBO, 2, 1, GL_FLOAT, sizeof(StarPoint), (void*) offsetof(StarPoint, size));
	starVAO.LinkIAttrib(starVBO, 3, 1, GL_UNSIGNED_INT, sizeof(StarPoint), (void*) offsetof(StarPoint, starType));
	starVAO.LinkIAttrib(starVBO, 4, 1, GL_UNSIGNED_INT, sizeof(StarPoint), (void*) offsetof(StarPoint, starIndex));

	starTypeCount = { 6, 16 };
	starSamplers = {GLint (largeStarsTex.TextureIndex), GLint(smallStarsTex.TextureIndex) };
}


void SkyRenderer::Render() {
	BindTextures();
	SetUniforms();

	glDisable(GL_DEPTH_TEST); // REVIEW THIS LATER TO MAKE SURE BLENDIGN IS CORRECT
	glEnable(GL_BLEND); 
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	dustRenderProgram.Activate();
	dustVAO.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	starRenderProgram.Activate();
	starVAO.Bind();
	glDrawArrays(GL_POINTS, 0, stars.size());
}


void SkyRenderer::BindTextures() {
	largeStarsTex.BindTex();
	smallStarsTex.BindTex();
}

// CHANGE SHADER UNIFORM NAMES LATER
void SkyRenderer::SetUniforms() {// CHANGE SHADER UNIFORM NAMES LATER
	camera.UploadInverseMatrix(dustRenderProgram, "ICameraMatrix");

	camera.UploadMatrix(starRenderProgram, "camMatrix"); // changwe
	camera.UploadPosition(starRenderProgram, "camPos");
	glUniform1uiv(glGetUniformLocation(starRenderProgram.ID, "starTypeCount"), 2, starTypeCount.data());
	glUniform1iv(glGetUniformLocation(starRenderProgram.ID, "starSamplers"), 2, starSamplers.data());
}