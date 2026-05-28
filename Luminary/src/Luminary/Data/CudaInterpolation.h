#pragma once

#include <vector>


struct Vec3f {
    float x;
    float y;
    float z;
};

struct GpuSplineData {
    // empty for now is okay
};

void EvaluateInterpolatedPositionsCUDA(
    const GpuSplineData& splineData,
    int objectCount,
    int frameCount,
    float fps,
    float startTime,
    std::vector<Vec3f>& outputPositions
);