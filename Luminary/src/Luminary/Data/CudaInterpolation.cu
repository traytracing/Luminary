#include <cuda_runtime.h>
#include "CudaInterpolation.h"

__device__ Vec3f evalSplineDevice() {
    return Vec3f{ 0.0f, 0.0f, 0.0f };
}

__global__ void interpolateKernel() {
    Vec3f v = evalSplineDevice();
}

void EvaluateInterpolatedPositionsCUDA(
    const GpuSplineData& splineData,
    int objectCount,
    int frameCount,
    float fps,
    float startTime,
    std::vector<Vec3f>& outputPositions
) {
    interpolateKernel << <1, 1 >> > ();
    cudaDeviceSynchronize();
}