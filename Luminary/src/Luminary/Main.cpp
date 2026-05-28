#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "Platform/OpenGL/imgui_impl_glfw.h"
#include "Platform/OpenGL/imgui_impl_opengl3.h"


#include <vector>

#include "Simulation.h"
#include "Luminary/RenderSystem.h"
#include <iostream>
#include <filesystem>

const unsigned int width = 1920;
const unsigned int height = 1080;

void printData(int tsIN,
	const std::vector<double>& timesIN,
	int obcIN,
	const std::vector<std::vector<glm::vec3>>& positionsIN)
{
	std::cout << "tsIN: " << tsIN << "\n";

	std::cout << "timesIN (" << timesIN.size() << "): ";
	for (double t : timesIN)
		std::cout << t << " ";
	std::cout << "\n";

	std::cout << "obcIN: " << obcIN << "\n";

	std::cout << "positionsIN (" << positionsIN.size() << " outer vectors):\n";
	for (size_t i = 0; i < positionsIN.size(); ++i) {
		std::cout << "  [" << i << "] (" << positionsIN[i].size() << " vec3s): ";
		for (const glm::vec3& v : positionsIN[i]) {
			std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ") ";
		}
		std::cout << "\n";
	}
}


int main() {




	/*
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // and swapinterval 0

	GLFWwindow* window = glfwCreateWindow(width, height, "N-VISUALIZATION", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL();

	// Init ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	// Choose GLSL version that matches your OpenGL version
	
	const char* glsl_version = "#version 460";
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	

	std::fstream fout;

	Simulation sim;

	
	int tsIN;
	std::vector<double> timesIN;
	int obcIN;

	std::cout << "Working directory: "
		<< std::filesystem::current_path()
		<< std::endl;

	fout.open("InputData.nbody", std::ios::in | std::ios::binary);
	if (!fout) std::cerr << "data error";
	fout.read(reinterpret_cast<char*> (&tsIN), sizeof(int));
	timesIN.resize(tsIN);
	fout.read(reinterpret_cast<char*> (timesIN.data()), tsIN * sizeof(double));
	fout.read(reinterpret_cast<char*> (&obcIN), sizeof(int));
	std::vector<glm::vec3> a(tsIN * obcIN);
	fout.read(reinterpret_cast<char*>(a.data()), a.size() * sizeof(glm::vec3));

	std::vector<std::vector<glm::vec3>> positionsIN(tsIN, std::vector<glm::vec3>(obcIN));
	for (int i = 0; i < tsIN; ++i)
		std::copy(a.begin() + i * obcIN,
			a.begin() + (i + 1) * obcIN,
			positionsIN[i].begin());
	fout.close();
	//printData(tsIN, timesIN, obcIN, positionsIN);

	RenderSystem renderSystem(window, width, height);

	renderSystem.InputData(std::move(tsIN), std::move(timesIN), std::move(obcIN), std::move(positionsIN));


	renderSystem.Begin();



	glfwTerminate(); */
	return 0;
}

/*std::vector<std::pair<std::vector<glm::vec3>, GLfloat>> outSim = sim.makeFullSim();

	// rebuild the fout
	fout.open("InputData.nbody", std::ios::out | std::ios::binary);
	int tsOut = outSim.size();
	fout.write(reinterpret_cast<char*> (&tsOut), sizeof(int));
	std::vector<double> timesOut;
	for (int i = 0; i < outSim.size(); i++) {
		timesOut.push_back(outSim[i].second);
	}
	fout.write(reinterpret_cast<char*>(timesOut.data()), sizeof(double) * timesOut.size());

	int obcOut = outSim[0].first.size();
	fout.write(reinterpret_cast<char*>(&obcOut), sizeof(int));
	for (int i = 0; i < outSim.size(); i++) {
		std::vector<glm::vec3> curout;
		curout = outSim[i].first;
		fout.write(reinterpret_cast<char*>(curout.data()), sizeof(glm::vec3) * curout.size());
	}
	fout.close();*/
