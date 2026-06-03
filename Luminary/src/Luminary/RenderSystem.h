#ifndef RENDER_SYSTEM_CLASS
#define RENDER_SYSTEM_CLASS


#include "InputManager.h"
#include "Renderer/SkyRenderer.h"
#include "Renderer/NbodyRenderer.h"

#include "Core/Camera.h"
#include "Data/InputDataProcessor.h"
#include "Settings.h"

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
	InputDataProcessor inputDataProcessor;

	NbodyRenderer nbodyRenderer {POVCamera, inputDataProcessor.GetSSBORef()};
	SkyRenderer skyRenderer {1000, SkyCamera};
	RenderProgram gridRenderProgram{ "grid.vert", "grid.frag" };
	VAO vaoooo;

	void Render();
};

#endif