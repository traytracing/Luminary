#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Simulation.h"
#include "Luminary/RenderSystem.h"

int main() {
	glfwInit();

    Simulation sim;
    sim.MakeSim("InputData", Simulation::SimType::Spiral);
	
	RenderSystem renderSystem;
	renderSystem.Run();

	glfwTerminate();
	return 0;
}