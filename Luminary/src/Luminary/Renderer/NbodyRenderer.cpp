#include "NbodyRenderer.h"
#include <iostream>
#include <algorithm>
#include <random>
#include "Luminary/Core/PIMath.h"
#include <glm/gtc/type_ptr.hpp>
#include "Luminary/AxisPoint.h"

// MAKE THIS LATER
NbodyRenderer::~NbodyRenderer() {

}
// change names
std::vector<glm::vec2> bNormals;
std::vector<GLfloat> rSeeds;
std::vector<GLfloat> masses;
GLfloat trailWidth = 5.0f;
GLfloat axisWidth = 2.3f;

int objectCount = 20000;

// change hte whole thing later
NbodyRenderer::NbodyRenderer(Camera& POVCamera, SSBO& positionSSBO) : POVCamera(POVCamera), positionSSBO(positionSSBO) {
	static bool initialized = false;
	if (initialized) throw std::runtime_error("NbodyRenderer already initialized");
	initialized = true;

	
	//blackholeVAO.LinkAttrib(blackholeVBO, 0, 4, GL_FLOAT, sizeof(BlackholePoint), (void*)0);
	blackholeVAO.LinkAttrib(blackholeVBO, 0, 2, GL_FLOAT, sizeof(BlackholePoint), (void*)0);
	blackholeVAO.LinkAttrib(blackholeVBO, 1, 1, GL_FLOAT, sizeof(BlackholePoint), (void*)offsetof(BlackholePoint, mass));
	blackholeVAO.LinkAttrib(blackholeVBO, 2, 2, GL_FLOAT, sizeof(BlackholePoint), (void*)offsetof(BlackholePoint, size));
	blackholeVAO.LinkAttrib(blackholeVBO, 3, 1, GL_FLOAT, sizeof(BlackholePoint), (void*)offsetof(BlackholePoint, seed));

	trailVAO.LinkAttrib(trailVBO, 0, 3, GL_FLOAT, sizeof(TrailPoint), (void*)0);
	trailVAO.LinkAttrib(trailVBO, 1, 1, GL_FLOAT, sizeof(TrailPoint), (void*)(3 * sizeof(GLfloat)));
	trailVAO.LinkAttrib(trailVBO, 2, 4, GL_FLOAT, sizeof(TrailPoint), (void*)(4 * sizeof(GLfloat)));

	std::default_random_engine rng2(123);
	std::uniform_real_distribution<float> U2(0.0f, 1.0f);
	bNormals.reserve(objectCount); // should be in set data
	rSeeds.reserve(objectCount);
	masses.reserve(objectCount);
	for (int i = 0; i < objectCount; ++i) {
		float theta = TAU * U2(rng2);
		float phi = acosf(1.0f - 2.0f * U2(rng2));
		float rseed = 20000 * U2(rng2);
		rSeeds.push_back(rseed);
		bNormals.push_back(glm::vec2(phi, theta));
		masses.push_back(std::log((i + 1) * 50.0f));
	}

	//masses[34] = 50.0f;
}
extern int fps;
extern int totalSimTime;
 std::vector<std::pair<std::vector<glm::vec3>, GLfloat>> i_objectsPositionsAcrossTime;

/*
std::vector<NbodyRenderer::InterpolatedGoodData> data;
// take move semantics
void NbodyRenderer::InterpolateData(int timestampsCount, std::vector<double> timestamps, int objectCount, std::vector<std::vector<glm::vec3>> positions) {
	// REWRITE EVERYTHING LATER
	std::vector<std::vector<glm::vec3>> position;
	for (int i = 0; i < objectCount; ++i) {
		std::vector<glm::vec3> cur;
		for (int j = 0; j < timestampsCount; ++j) {
			cur.push_back(positions[j][i]);
		}
		position.push_back(cur);
	}
	
	
	
	for (int i = 0; i < objectCount; ++i) {
		InterpolatedGoodData cur;
		cur.setDataPoints(timestamps, position[i]);
		data.emplace_back(cur);
	}

	double sTime = timestamps[0];
	for (int i = 0; i < fps * totalSimTime; ++i) {
		sTime += 1.0 / fps;

		std::vector<glm::vec3> pos; // update with pairs for unique id etc
		for (int j = 0; j < objectCount; ++j) {
			pos.push_back(data[j].eval(sTime));
		}
		i_objectsPositionsAcrossTime.emplace_back(
			std::pair(pos, sTime)
		);

	}
	std::cout << "Total Unique Frames: " << i_objectsPositionsAcrossTime.size() << std::endl;
}
*/

extern GLfloat simTime;
extern int frameIndex;
extern int curView;

void NbodyRenderer::SetUniforms() {
	blackholeRenderProgram.Activate();
	POVCamera.UploadMatrix(blackholeRenderProgram, "CameraMatrix");
	POVCamera.UploadPosition(blackholeRenderProgram, "CameraPosition");
	glUniform1f(glGetUniformLocation(blackholeRenderProgram.ID, "TIME"), frameIndex / 165.0f); // multply by timespeed in frag shader
	
	trailRenderProgram.Activate();
	POVCamera.UploadMatrix(trailRenderProgram, "CameraMatrix");
}
//MOVE CURVIEW TO INPUTMANAGER 
	//POVCamera.object = i_objectsPositionsAcrossTime[frameIndex].first[curView]; // 4 // 58
	//POVCamera.lookat = i_objectsPositionsAcrossTime[frameIndex].first[1];
	//

GLfloat trailTime = 2.0f;
glm::vec4 trailColor = glm::vec4(0.043f, 0.031f, 0.118f, 1.0f);


// take in time
void NbodyRenderer::Render() {
	if (frameIndex < 0 || frameIndex > 16499) { POVCamera.object = glm::vec3(1);
	return; }

	//POVCamera.object = i_objectsPositionsAcrossTime[frameIndex].first[curView];
	//if (std::isnan(POVCamera.object.x)) 
	//	POVCamera.object = glm::vec3(1);
	//FIX LATER THIS IS WHY ITS BAD

	SetUniforms();
	objects.clear(); lines.clear();

	for (int i = 0; i < objectCount; i++) {
		BlackholePoint curObject;

		//if (std::isnan(curObject.position.x)) continue;
		curObject.bNormal = bNormals[i];
		curObject.size = glm::vec2(0.2f); //0.035f
		curObject.mass = masses[i];
		curObject.seed = rSeeds[i];

		objects.push_back(curObject);
	}
	
	blackholeVBO.SetData(objects.data(), objects.size() * sizeof(BlackholePoint));

	
	blackholeRenderProgram.Activate();

	
	glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);


	blackholeRenderProgram.Activate();
	positionSSBO.BindBase(0);
	blackholeVAO.Bind();
	glDrawArrays(GL_POINTS, 0, objects.size());

	/*
	for (int i = 0; i < objectCount; i++) {
		for (int j = frameIndex; j >= 0 && i_objectsPositionsAcrossTime[frameIndex].second - i_objectsPositionsAcrossTime[j].second <= trailTime; j-=5) {
			TrailPoint cur;
			cur.bpos = glm::vec3(i_objectsPositionsAcrossTime[j].first[i]);
			if (std::isnan(cur.bpos.x)) break;

			if (j >= frameIndex) cur.renderbool = -1.0f;
			else cur.renderbool = 1.0f;
			GLfloat fade = ((i_objectsPositionsAcrossTime[frameIndex].second - i_objectsPositionsAcrossTime[j].second) / trailTime);
			cur.color = glm::vec4(0.3f, 0.5f, 0.9f, 1.0f - fade);
			cur.color = glm::vec4(0.043f, 0.031f, 0.118f, 1.0f - fade);
			cur.color = trailColor;
			cur.color.a = trailColor.a * (1.0f - fade);
			lines.emplace_back(cur);
		}
	}
	
	trailVBO.SetData(lines.data(), lines.size() * sizeof(TrailPoint));
	



	glEnable(GL_DEPTH_TEST); glDepthMask(GL_FALSE); // trail over trail ask danny
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(trailWidth);

	trailRenderProgram.Activate();
	trailVAO.Bind();
	glDrawArrays(GL_LINE_STRIP, 0, lines.size());

	RenderProgram a{ "axis.vert", "axis.geom", "axis.frag" };
	POVCamera.UploadMatrix(a, "CameraMatrix");
	glUniform1f(glGetUniformLocation(a.ID, "Scale"), 0.35f);
	std::vector<AxisPoint> axisPoints;


	glEnable(GL_DEPTH_TEST); glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(axisWidth);
	for (int i{}; i < objectCount; ++i) {
		AxisPoint cur;
		cur.position = i_objectsPositionsAcrossTime[frameIndex].first[i];
		if (std::isnan(cur.position.x)) continue;
		cur.bNormal = bNormals[i];
		axisPoints.emplace_back(cur);
	}
	VBO VBO3{ GL_DYNAMIC_DRAW };
	VBO3.SetData(axisPoints.data(), axisPoints.size() * sizeof(AxisPoint));
	VAO VAO230203;
	VAO230203.LinkAttrib(VBO3, 0, 3, GL_FLOAT, sizeof(AxisPoint), (void*)0);
	VAO230203.LinkAttrib(VBO3, 1, 2, GL_FLOAT, sizeof(AxisPoint), (void*) offsetof(AxisPoint, bNormal));


	a.Activate();
	VAO230203.Bind();
	glDrawArrays(GL_POINTS, 0, axisPoints.size());
	*/
}