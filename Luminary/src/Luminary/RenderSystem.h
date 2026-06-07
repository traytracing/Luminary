#pragma once

#include "InputManager.h"
#include "Settings.h"
#include "Core/Camera.h"
#include "Renderer/SkyRenderer.h"
#include "Renderer/NbodyRenderer.h"
#include "Renderer/GridRenderer.h"
#include "VideoManager.h"
#include "Gui.h"

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
	void Cleanup();

	InputManager inputManager{ settings, window };
	void Link();
	Gui gui{ settings, window };

	void Loop();
	void LaunchMenuLoop();
	void InSceneLoop();
	void RenderingLoop();

	Camera POVCamera{ settings };
	Camera SkyCamera{ settings };
	InputDataProcessor inputDataProcessor{ settings };
	NbodyRenderer nbodyRenderer{ POVCamera, settings, inputDataProcessor };
	SkyRenderer skyRenderer{ 1000, SkyCamera };
	GridRenderer gridRenderer{ POVCamera };

	VideoManager videoManager{ settings };
};