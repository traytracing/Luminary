#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Simulation.h"
#include "Luminary/RenderSystem.h"

#include <fstream>

void WriteBouncing3DLumenSim(const std::string& filepath)
{
    const int objectCount = 1000;
    const float durationSeconds = 60.0f;
    const float dataFPS = 120.0f;
    const int dataFrameCount = static_cast<int>(durationSeconds * dataFPS);

    const float dt = 1.0f / dataFPS;

    const float boxHalfSize = 50.0f;
    const float minSpeed = 2.0f;
    const float maxSpeed = 12.0f;

    std::ofstream out(filepath, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open output file: " << filepath << std::endl;
        return;
    }

    out.write(reinterpret_cast<const char*>(&objectCount), sizeof(int));
    out.write(reinterpret_cast<const char*>(&dataFrameCount), sizeof(int));

    std::vector<float> times(dataFrameCount);
    for (int frame = 0; frame < dataFrameCount; frame++) {
        times[frame] = static_cast<float>(frame) * dt;
    }

    out.write(
        reinterpret_cast<const char*>(times.data()),
        times.size() * sizeof(float)
    );

    std::vector<glm::vec3> positions(objectCount);
    std::vector<glm::vec3> velocities(objectCount);
    std::vector<glm::vec4> outputFrame(objectCount);

    std::mt19937 rng(1337);

    std::uniform_real_distribution<float> posDist(-boxHalfSize, boxHalfSize);
    std::uniform_real_distribution<float> velDist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> speedDist(minSpeed, maxSpeed);

    for (int i = 0; i < objectCount; i++) {
        positions[i] = glm::vec3(
            posDist(rng),
            posDist(rng),
            posDist(rng)
        );

        glm::vec3 dir(
            velDist(rng),
            velDist(rng),
            velDist(rng)
        );

        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        if (len < 0.0001f) {
            dir = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        else {
            dir /= len;
        }

        velocities[i] = dir * speedDist(rng);
    }

    for (int frame = 0; frame < dataFrameCount; frame++) {
        if (frame % 60 == 0) {
            std::cout << "Writing frame " << frame << " / " << dataFrameCount << std::endl;
        }

        for (int i = 0; i < objectCount; i++) {
            outputFrame[i] = glm::vec4(
                positions[i].x,
                positions[i].y,
                positions[i].z,
                1.0f
            );
        }

        out.write(
            reinterpret_cast<const char*>(outputFrame.data()),
            outputFrame.size() * sizeof(glm::vec4)
        );

        for (int i = 0; i < objectCount; i++) {
            positions[i] += velocities[i] * dt;

            if (positions[i].x < -boxHalfSize) {
                positions[i].x = -boxHalfSize;
                velocities[i].x *= -1.0f;
            }
            else if (positions[i].x > boxHalfSize) {
                positions[i].x = boxHalfSize;
                velocities[i].x *= -1.0f;
            }

            if (positions[i].y < -boxHalfSize) {
                positions[i].y = -boxHalfSize;
                velocities[i].y *= -1.0f;
            }
            else if (positions[i].y > boxHalfSize) {
                positions[i].y = boxHalfSize;
                velocities[i].y *= -1.0f;
            }

            if (positions[i].z < -boxHalfSize) {
                positions[i].z = -boxHalfSize;
                velocities[i].z *= -1.0f;
            }
            else if (positions[i].z > boxHalfSize) {
                positions[i].z = boxHalfSize;
                velocities[i].z *= -1.0f;
            }
        }
    }

    out.close();

    const double bytes =
        double(sizeof(int) * 2) +
        double(sizeof(float)) * dataFrameCount +
        double(sizeof(glm::vec4)) * objectCount * dataFrameCount;

    std::cout
        << "Wrote bouncing 3D lumen sim: " << filepath << "\n"
        << "objectCount=" << objectCount << "\n"
        << "frameCount=" << dataFrameCount << "\n"
        << "durationSeconds=" << durationSeconds << "\n"
        << "dataFPS=" << dataFPS << "\n"
        << "approxFileSizeMB=" << bytes / (1024.0 * 1024.0)
        << std::endl;
}

int main() {
	glfwInit();

    //WriteBouncing3DLumenSim("InputData.lumen");

	/*
	Simulation sim;
	std::vector<std::pair<std::vector<glm::vec4>, GLfloat>> outSim = sim.makeFullSim();

	// rebuild the fout
	std::fstream fout;
	fout.open("InputData.lumen", std::ios::out | std::ios::binary);


	int obcOut = outSim[0].first.size();
	fout.write(reinterpret_cast<char*>(&obcOut), sizeof(int));

	int tsOut = outSim.size();
	fout.write(reinterpret_cast<char*> (&tsOut), sizeof(int));

	std::vector<float> timesOut;
	for (int i = 0; i < outSim.size(); i++) {
		timesOut.push_back(outSim[i].second);
	}
	fout.write(reinterpret_cast<char*>(timesOut.data()), sizeof(float) * timesOut.size());

	for (int i = 0; i < outSim.size(); i++) {
		std::vector<glm::vec4> curout;
		curout = outSim[i].first;
		fout.write(reinterpret_cast<char*>(curout.data()), sizeof(glm::vec4) * curout.size());
	}
	fout.close();*/
	
	RenderSystem renderSystem;
	renderSystem.Run();

	glfwTerminate();

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
