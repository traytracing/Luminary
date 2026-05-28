#ifndef RENDER_SYSTEM_CLASS
#define RENDER_SYSTEM_CLASS


#include "InputManager.h"
#include "Renderer/SkyRenderer.h"
#include "Renderer/NbodyRenderer.h"

#include "Core/Camera.h"

class RenderSystem {
public:
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;
	RenderSystem(RenderSystem&&) = delete;
	RenderSystem& operator=(RenderSystem&&) = delete;

public:
	~RenderSystem();
	RenderSystem(GLFWwindow* window, GLint iWidth, GLint iHeight); // change iwndow later

	void InputData(int timestampsCount, std::vector<double> timestamps, int objectCount, std::vector<std::vector<glm::vec3>> positions);
	void Begin();




private:
	GLFWwindow* window;
	InputManager inputManager;

	Camera POVCamera;
	Camera SkyCamera;

	NbodyRenderer nbodyRenderer {POVCamera};
	SkyRenderer skyRenderer {1000, SkyCamera};
	RenderProgram gridRenderProgram{ "grid.vert", "grid.frag" };
	VAO vaoooo;

	void Render();
};

#endif