#pragma once

#include "InputManager.h"
#include "Settings.h"
#include "Core/Camera.h"
#include "Data/InputDataProcessor.h"
#include "Renderer/SkyRenderer.h"
#include "Renderer/NbodyRenderer.h"
#include "Renderer/GridRenderer.h"

class RenderSystem {
public:
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;
	RenderSystem(RenderSystem&&) = delete;
	RenderSystem& operator=(RenderSystem&&) = delete;
public:
	~RenderSystem();
	RenderSystem();

	void Run();
private:
	Settings settings{};
	GLFWwindow* MakeWindow();
	GLFWwindow* window = MakeWindow();

	Camera POVCamera;
	Camera SkyCamera;

	InputManager inputManager{POVCamera, SkyCamera};
	InputDataProcessor inputDataProcessor{ settings };

	NbodyRenderer nbodyRenderer{ POVCamera, inputDataProcessor.GetSSBORef() };
	SkyRenderer skyRenderer{ 1000, SkyCamera };
	GridRenderer gridRenderer{ POVCamera };
	void Render();
};