#include "Simulation.h"
#include <iostream>
#include <random>
#include <fstream>
#include <utility>
#include <cmath>
#include "Core/AssetPath.h"


bool Simulation::WriteSimToFile(const std::string& filename, const std::vector<std::pair<std::vector<glm::vec4>, GLfloat>>& OutSim) {
    if (!OutSim.size()) return false;
    
    std::fstream fout;
    fout.open(GetAssetPath("inputdata/" + filename + ".lumen"), std::ios::out | std::ios::binary);

    int objectCount = OutSim[0].first.size();
    fout.write(reinterpret_cast<char*>(&objectCount), sizeof(int));

    int dataFrameCount = OutSim.size();
    fout.write(reinterpret_cast<char*> (&dataFrameCount), sizeof(int));

    std::vector<float> timesOut;
    for (int i = 0; i < OutSim.size(); i++) {
        timesOut.push_back(OutSim[i].second);
    }
    fout.write(reinterpret_cast<char*>(timesOut.data()), sizeof(float) * timesOut.size());

    for (int i = 0; i < OutSim.size(); i++) {
        std::vector<glm::vec4> curout;
        curout = OutSim[i].first;
        fout.write(reinterpret_cast<char*>(curout.data()), sizeof(glm::vec4) * curout.size());
    }
    fout.close();

    const double bytes =
        double(sizeof(int) * 2) +
        double(sizeof(float)) * dataFrameCount +
        double(sizeof(glm::vec4)) * objectCount * dataFrameCount;

    std::cout
        << "Wrote lumen sim to " << filename << ".lumen" << "\n"
        << "objectCount=" << objectCount << "\n"
        << "frameCount=" << dataFrameCount << "\n"
        << "durationSeconds=" << timesOut.back() - timesOut.front() << "\n"
        << "approxFileSizeMB=" << bytes / (1024.0 * 1024.0)
        << std::endl;
    return true;
}
void Simulation::MakeSim(const std::string& filename, const SimType& type) {
    switch (type) {
    case SimType::Original:
        if (WriteSimToFile(filename, MakeOriginalSim())) return;
        break;
    case SimType::Spiral:
        if (WriteSimToFile(filename, MakeSpiralSim())) return;
        break;
    }
    
    std::cerr << "Simulation Error" << std::endl;
}

std::vector<std::pair<std::vector<glm::vec4>, GLfloat>> Simulation::MakeOriginalSim() {
    const int totalSimTime = 30;
    const int objectCount = 30;
    const GLfloat k = 10.0f;

    std::random_device rd;
    int seed = rd();

    std::cout << "Seed: " << seed << std::endl;
    std::mt19937 gen(seed);
    std::uniform_real_distribution<float> disPos(-20.0f, 20.0f);
    std::uniform_real_distribution<float> disVel(-0.5f, 0.5f);

    // create objects initial position + vel
    std::vector<std::pair<glm::vec4, glm::vec4>> initialObjects;
    for (int i = 0; i < objectCount; i++) {
        glm::vec4 pos = glm::vec4(disPos(gen), disPos(gen), disPos(gen), 0);
        glm::vec4 vel = glm::vec4(disVel(gen), disVel(gen), disVel(gen), 0);
        initialObjects.push_back(std::pair(pos, vel));
    }

    std::vector<std::pair<std::vector<std::pair<glm::vec4, glm::vec4>>, GLfloat>> sim;
    sim.push_back(std::pair(initialObjects, 0.0f));

    // simulate
    GLfloat simTime = 0.0f;
    float deltaTime = 0.0000000001f;

    auto updateSim = [&]()
    {
        std::vector<std::pair<glm::vec4, glm::vec4>> allNew;
        float dtgeo = std::pow(10, 20);
        for (int i = 0; i < sim.back().first.size(); ++i) {
            glm::vec4 force{};
            for (int j = 0; j < sim.back().first.size(); ++j) {
                if (j == i) continue;
                glm::vec4 cur = sim.back().first[i].first;
                glm::vec4 comp = sim.back().first[j].first;
                glm::vec4 diff = comp - cur;

                float relDist = glm::length(diff);
                force += GLfloat(k / pow(relDist, 2)) * glm::normalize(diff);
                float relVel = glm::length(sim.back().first[i].second - sim.back().first[j].second) / glm::length(force);
                float candidate = relDist / relVel;
                if (candidate < dtgeo) {
                    dtgeo = candidate;
                }
            }

            glm::vec4 newVel = sim.back().first[i].second + force * deltaTime;
            glm::vec4 newPos = sim.back().first[i].first + newVel * deltaTime;
            allNew.push_back(std::pair(newPos, newVel));
        }

        sim.push_back(std::pair(allNew, simTime));
        return dtgeo * 0.9f; // 0.25 good
    };

    while (simTime < totalSimTime) {
        deltaTime = updateSim();
        simTime += deltaTime;
    }

    // convert format
    std::vector<std::pair<std::vector<glm::vec4>, GLfloat>> OutSim;
    for (int i = 0; i < sim.size(); i++) {
        std::vector<glm::vec4> curObjs;
        for (int j = 0; j < sim[i].first.size(); ++j) {
            curObjs.push_back(sim[i].first[j].first);
        }

        OutSim.push_back(std::pair(curObjs, sim[i].second));
    }

    return OutSim;
}
std::vector<std::pair<std::vector<glm::vec4>, GLfloat>> Simulation::MakeSpiralSim() {
    const int objectCount = 20000;
    const int frameCount = 2000;
    const GLfloat fps = 120.0f;

    const GLfloat pi = 3.14159265358979323846f;
    const GLfloat twoPi = 2.0f * pi;

    std::vector<std::pair<std::vector<glm::vec4>, GLfloat>> sim;
    sim.reserve(frameCount);

    std::mt19937 rng(12345);

    const int armCount = 4;

    // Scale radius outward as object count increases.
    // sqrt scaling keeps density fairly stable.
    const GLfloat baseRadiusScale = 1.2f;
    const GLfloat maxRadius = baseRadiusScale * std::sqrt(static_cast<GLfloat>(objectCount));

    std::uniform_real_distribution<GLfloat> armOffsetDist(-0.3f, 0.3f);
    std::uniform_real_distribution<GLfloat> heightDist(-0.5f, 0.5f);
    std::uniform_real_distribution<GLfloat> jitterDist(-0.15f, 0.15f);

    std::vector<GLfloat> baseRadius(objectCount);
    std::vector<GLfloat> baseAngle(objectCount);
    std::vector<GLfloat> baseHeight(objectCount);
    std::vector<GLfloat> angularSpeed(objectCount);

    for (int i = 0; i < objectCount; ++i) {
        GLfloat u = (static_cast<GLfloat>(i) + 1.0f) / static_cast<GLfloat>(objectCount);

        // sqrt(u) gives a more even disk density.
        GLfloat r = maxRadius * std::sqrt(u);

        int arm = i % armCount;

        baseRadius[i] = r;
        baseHeight[i] = heightDist(rng) * (0.15f + 0.02f * r);

        // Spiral arm formula
        baseAngle[i] =
            (twoPi * static_cast<GLfloat>(arm) / static_cast<GLfloat>(armCount)) +
            r * 0.32f +
            armOffsetDist(rng);

        // Inner stars rotate faster.
        angularSpeed[i] = 3.5f / std::sqrt(r + 1.0f);
    }

    for (int frame = 0; frame < frameCount; ++frame) {
        GLfloat t = static_cast<GLfloat>(frame) / fps;

        std::vector<glm::vec4> positions;
        positions.reserve(objectCount);

        for (int i = 0; i < objectCount; ++i) {
            GLfloat r = baseRadius[i];

            GLfloat theta = baseAngle[i] + angularSpeed[i] * t;

            // Slight breathing motion
            GLfloat pulse = 1.0f + 0.03f * std::sin(t * 1.8f + r * 0.4f);

            GLfloat x = std::cos(theta) * r * pulse;
            GLfloat z = std::sin(theta) * r * pulse;

            GLfloat y =
                baseHeight[i] +
                0.08f * std::sin(theta * 2.0f + t * 1.2f);

            // Tiny jitter to keep it from looking too perfect
            x += jitterDist(rng) * 0.05f;
            y += jitterDist(rng) * 0.03f;
            z += jitterDist(rng) * 0.05f;

            positions.emplace_back(x, y, z, static_cast<GLfloat>(i));
        }

        sim.emplace_back(std::move(positions), t);
    }

    return sim;
}