#include "Simulation.h"
#include <iostream>

float Simulation::UpdateSim(std::vector<std::pair<std::vector<std::pair<glm::vec3, glm::vec3>>, GLfloat>>& sim, GLfloat deltaTime, GLfloat simTime) {
	std::vector<std::pair<glm::vec3, glm::vec3>> allNew;
	float dtgeo = std::pow(10, 20);
	for (int i = 0; i < sim.back().first.size(); ++i) {
		glm::vec3 force = glm::vec3(0.0f);
		

		for (int j = 0; j < sim.back().first.size(); ++j) {
			if (j == i) continue;
			glm::vec3 cur = sim.back().first[i].first;
			glm::vec3 comp = sim.back().first[j].first;
			glm::vec3 diff = comp - cur;
			
			float relDist = glm::length(diff);
			force += GLfloat(k / pow(relDist, 2)) * glm::normalize(diff);
			float relVel = glm::length(sim.back().first[i].second - sim.back().first[j].second) / glm::length(force);
			dtgeo = std::min(relDist / relVel, dtgeo);
		}
		//force +=  -1.0f * glm::normalize(sim.back().first[i].first); // centripital force 

		glm::vec3 newVel = sim.back().first[i].second + force * deltaTime;
		glm::vec3 newPos = sim.back().first[i].first + newVel * deltaTime;
		allNew.push_back(std::pair(newPos, newVel));
	}

	sim.push_back(std::pair(allNew, simTime));
	return dtgeo * 0.6f; // 0.25 good
}


#include <random>
extern int totalSimTime;
int randomNum = -1231495304;
extern int objectCount;
std::vector<std::pair<std::vector<glm::vec3>, GLfloat>> Simulation::makeFullSim() {
	std::vector<std::pair<std::vector<std::pair<glm::vec3, glm::vec3>>, GLfloat>> sim;

	std::random_device rd;
	int randomNum = rd();
	//randomNum = 1053480021;
	//randomNum = -1067049185;
	//randomNum = 1465987027;
	//randomNum = 242667232; // 8
	randomNum = -1231495304; //16 & 17
	//randomNum = -1288244767; //BAD
	//randomNum = -1118661049;
	std::cout << randomNum << std::endl;
	std::mt19937 gen(randomNum);
	std::uniform_real_distribution<float> angleDist(0.0f, 6.14);
	std::uniform_real_distribution<float> radiusDist(30.0f, 40.0f);       // Disc radius
	std::uniform_real_distribution<float> speedDist(4.0f, 8.0f);
	std::uniform_real_distribution<float> disPos(-20.0f, 20.0f);
	std::uniform_real_distribution<float> disVel(-0.5f, 0.5f);
	std::uniform_real_distribution<float> disTime(0.0001f, 0.05f);

	//create objects initial position + vel
	std::vector<std::pair<glm::vec3, glm::vec3>> initialObjects;
	for (int i = 0; i < objectCount; i++) {
		float angle = angleDist(gen);
		float radius = radiusDist(gen);
		glm::vec3 pos = glm::vec3(std::cos(angle), std::sin(angle), 0.0f) * radius;

		float speed = speedDist(gen);
		glm::vec3 vel = glm::vec3(std::sin(angle), -std::cos(angle), 0.0f) * speed;

		pos = glm::vec3(disPos(gen), disPos(gen), disPos(gen));
		vel = glm::vec3(disVel(gen), disVel(gen), disVel(gen));
		initialObjects.push_back(std::pair(pos, vel));
	}
	sim.push_back(std::pair(initialObjects, 0.0f));
	

	
	//simulate 50 seconds
	GLfloat simTime = 0.0f;
	float nextdt = 0.0000000001f;
	int i = 0;
	while (simTime < totalSimTime) {
		simTime += nextdt;
		nextdt = UpdateSim(sim, nextdt, simTime);
		i++;
	}


	//convert format
	std::vector<std::pair<std::vector<glm::vec3>, GLfloat>> out;
	for (int i = 0; i < sim.size(); i++) {
		std::vector<glm::vec3> curObjs;
		for (int j = 0; j < sim[i].first.size(); ++j) {
			curObjs.push_back(sim[i].first[j].first);
		}
		
		out.push_back(std::pair(curObjs, sim[i].second));
	}

	// not safe
	for (int i = 3999; i < 8000; i++) {
		out[i].first[17] = glm::vec3(std::nan(""));
	}
	
	return out;
}