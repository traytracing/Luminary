#include "NbodyRenderer.h"
#include <random>
#include "Luminary/Core/PIMath.h"

NbodyRenderer::~NbodyRenderer() {
}
NbodyRenderer::NbodyRenderer(Camera& POVCamera, const Settings& SRF, InputDataProcessor& IDPR) : POVCamera(POVCamera), SRF(SRF), IDPR(IDPR) {
	static bool initialized = false;
	if (initialized) throw std::runtime_error("NbodyRenderer already initialized");
	initialized = true;

	blackholeVAO.LinkAttrib(blackholeVBO, 0, 2, GL_FLOAT, sizeof(ObjectAttributes), (void*)0);
	blackholeVAO.LinkAttrib(blackholeVBO, 1, 1, GL_FLOAT, sizeof(ObjectAttributes), (void*)offsetof(ObjectAttributes, mass));
	blackholeVAO.LinkAttrib(blackholeVBO, 2, 2, GL_FLOAT, sizeof(ObjectAttributes), (void*)offsetof(ObjectAttributes, size));
	blackholeVAO.LinkAttrib(blackholeVBO, 3, 1, GL_FLOAT, sizeof(ObjectAttributes), (void*)offsetof(ObjectAttributes, seed));

	axisVAO.LinkAttrib(blackholeVBO, 0, 2, GL_FLOAT, sizeof(ObjectAttributes), (void*)0);
}

void NbodyRenderer::UpdateObjectsAttributeData() {
	if (IDPR.GetObjectCount() <= objectAttribs.capacity) { return; }

	static std::default_random_engine rng2(123);
	static std::uniform_real_distribution<float> U2(0.0f, 1.0f);

	int objectCount = IDPR.GetObjectCount();

	for (int i = objectAttribs.capacity; i < objectCount; ++i) {
		float theta = TAU * U2(rng2);
		float phi = acosf(1.0f - 2.0f * U2(rng2));
		float rseed = 20000 * U2(rng2);
		objectAttribs.attributes.emplace_back(glm::vec2(phi, theta), std::log((i + 1) * 50.0f), glm::vec2(0.2f), rseed);
	}
	objectAttribs.capacity = objectCount;
	blackholeVBO.SetData(objectAttribs.attributes.data(), objectAttribs.attributes.size() * sizeof(ObjectAttributes));
}
void NbodyRenderer::SetUniforms() {
	objectRenderProgram.Activate();
	POVCamera.UploadMatrix(objectRenderProgram, "CameraMatrix");
	POVCamera.UploadPosition(objectRenderProgram, "CameraPosition");
	glUniform1f(glGetUniformLocation(objectRenderProgram.ID, "TIME"), IDPR.lf.chunk.renderFrameIndex / IDPR.lf.chunk.fps); // multply by timespeed in frag shader
	
	trailRenderProgram.Activate();
	POVCamera.UploadMatrix(trailRenderProgram, "CameraMatrix");
	glUniform1i(glGetUniformLocation(trailRenderProgram.ID, "ObjectCount"), IDPR.lf.objectCount);
	glUniform1i(glGetUniformLocation(trailRenderProgram.ID, "TrailFrameCount"), IDPR.trailFrameCount);
	glUniform1i(glGetUniformLocation(trailRenderProgram.ID, "TrailWriteFrame"), IDPR.trailWriteFrame);
	glUniform1i(glGetUniformLocation(trailRenderProgram.ID, "ValidTrailFrameCount"), IDPR.validTrailFrameCount);
	glUniform4f(glGetUniformLocation(trailRenderProgram.ID, "TrailColor"), SRF.trailColor.x, SRF.trailColor.y, SRF.trailColor.z, SRF.trailColor.w);

	POVCamera.UploadMatrix(axisRenderProgram, "CameraMatrix");
	glUniform1f(glGetUniformLocation(axisRenderProgram.ID, "Scale"), 0.35f);
}
void NbodyRenderer::Render() {
	// object
	UpdateObjectsAttributeData();

	SetUniforms();

	objectRenderProgram.Activate();
	IDPR.GetSSBORef().BindBase(0);
	blackholeVAO.Bind();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDrawArrays(GL_POINTS, 0, IDPR.GetObjectCount());
	
	// sky
	if (IDPR.validTrailFrameCount >= 2 && IDPR.lf.objectCount > 0) {
		trailRenderProgram.Activate();
		IDPR.trailHistorySSBO.BindBase(1);
		
		glEnable(GL_DEPTH_TEST); glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(SRF.trailWidth);

		glDrawArraysInstanced(GL_LINE_STRIP, 0, IDPR.validTrailFrameCount, IDPR.lf.objectCount);
	}

	// axis
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(SRF.axisWidth);

	axisRenderProgram.Activate();
	axisVAO.Bind();
	glDrawArrays(GL_POINTS, 0, IDPR.GetObjectCount());
}