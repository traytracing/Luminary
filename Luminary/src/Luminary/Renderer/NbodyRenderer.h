#pragma once

#include <vector>

#include "Luminary/Settings.h"
#include "Luminary/Data/InputDataProcessor.h"

#include "Luminary/Core/Camera.h"
#include "Luminary/Core/RenderProgram.h"
#include "Luminary/Core/VAO.h"
#include "Luminary/Core/VBO.h"
#include "Luminary/Core/AssetPath.h"

#include "Luminary/Renderer/Structs/ObjectAttributes.h"

class NbodyRenderer {
	friend class RenderSystem;
public:
	NbodyRenderer(const NbodyRenderer&) = delete;
	NbodyRenderer& operator=(const NbodyRenderer&) = delete;
	NbodyRenderer(NbodyRenderer&&) = delete;
	NbodyRenderer& operator=(NbodyRenderer&&) = delete;
public:
	NbodyRenderer(Camera& POVCamera, const Settings& SRF, InputDataProcessor& IDPR);
	~NbodyRenderer();

	void Render();
private:
	const Settings& SRF;
	InputDataProcessor& IDPR;
	Camera& POVCamera;

	RenderProgram objectRenderProgram{ GetAssetPath("shaders/object.vert").c_str(), GetAssetPath("shaders/object.geom").c_str(), GetAssetPath("shaders/blackhole.frag").c_str() };
	RenderProgram trailRenderProgram{ GetAssetPath("shaders/line.vert").c_str(), GetAssetPath("shaders/line.frag").c_str() };
	RenderProgram axisRenderProgram{ GetAssetPath("shaders/axis.vert").c_str(), GetAssetPath("shaders/axis.geom").c_str(), GetAssetPath("shaders/axis.frag").c_str() };
	VAO blackholeVAO;
	VAO trailVAO;
	VAO axisVAO;
	VBO blackholeVBO{ GL_DYNAMIC_DRAW };

	struct ObjectsAttributeData {
		int capacity{};
		std::vector<ObjectAttributes> attributes;
	} objectAttribs;
	void UpdateObjectsAttributeData();
	void SetUniforms();
};