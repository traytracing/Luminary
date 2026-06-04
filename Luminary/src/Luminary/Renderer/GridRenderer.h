#ifndef GRID_RENDERER_CLASS_H
#define GRID_RENDERER_CLASS_H

#include <vector>
#include <array>

#include "Luminary/Core/RenderProgram.h"
#include "Luminary/Core/VAO.h"
#include "Luminary/Core/VBO.h"

#include "Luminary/Core/Camera.h"
#include "Luminary/Core/Texture.h"
#include "Luminary/Core/AssetPath.h"


class GridRenderer {
	friend class RenderSystem;
public:
	GridRenderer(const GridRenderer&) = delete;
	GridRenderer& operator=(const GridRenderer&) = delete;
	GridRenderer(GridRenderer&&) = delete;
	GridRenderer& operator=(GridRenderer&&) = delete;
public:
	GridRenderer(Camera& POVCamera);
	~GridRenderer();

	void Render();
	Camera& POVCamera;
private:
	RenderProgram gridRenderProgram{ GetAssetPath("shaders/grid.vert").c_str(), GetAssetPath("shaders/grid.frag").c_str() };
	VAO gridVAO;
};
#endif