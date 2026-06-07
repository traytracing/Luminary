#include "InputDataProcessorCuda.h"

#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#include <iostream>

static float* gDTimes = nullptr;
static size_t gDTimesCapacity = 0;

static float4* gDInputPositions = nullptr;
static size_t gDInputPositionsCapacity = 0;

static bool CheckCuda(cudaError_t result, const char* msg)
{
    if (result != cudaSuccess) {
        std::cerr << "CUDA Error: " << msg << " : "
            << cudaGetErrorString(result) << std::endl;
        return false;
    }

    return true;
}

static bool EnsureCudaBuffer(
    void** buffer,
    size_t* capacity,
    size_t requiredBytes,
    const char* name
) {
    if (requiredBytes <= *capacity) {
        return true;
    }

    if (*buffer) {
        if (!CheckCuda(cudaFree(*buffer), name)) {
            *buffer = nullptr;
            *capacity = 0;
            return false;
        }

        *buffer = nullptr;
        *capacity = 0;
    }

    if (!CheckCuda(cudaMalloc(buffer, requiredBytes), name)) {
        *buffer = nullptr;
        *capacity = 0;
        return false;
    }

    *capacity = requiredBytes;
    return true;
}

__device__ float4 Lerp(
    float renderTime,
    float t0,
    float t1,
    float4 p0,
    float4 p1
) {
    float denom = t1 - t0;

    if (denom == 0.0f) {
        return p0;
    }

    float a = (renderTime - t0) / denom;

    return make_float4(
        p0.x + (p1.x - p0.x) * a,
        p0.y + (p1.y - p0.y) * a,
        p0.z + (p1.z - p0.z) * a,
        1.0f
    );
}

__device__ float4 QuadraticInterpolate(
    float renderTime,
    float t0,
    float t1,
    float t2,
    float4 p0,
    float4 p1,
    float4 p2
) {
    float d0 = (t0 - t1) * (t0 - t2);
    float d1 = (t1 - t0) * (t1 - t2);
    float d2 = (t2 - t0) * (t2 - t1);

    if (d0 == 0.0f || d1 == 0.0f || d2 == 0.0f) {
        return p1;
    }

    float l0 = ((renderTime - t1) * (renderTime - t2)) / d0;
    float l1 = ((renderTime - t0) * (renderTime - t2)) / d1;
    float l2 = ((renderTime - t0) * (renderTime - t1)) / d2;

    return make_float4(
        p0.x * l0 + p1.x * l1 + p2.x * l2,
        p0.y * l0 + p1.y * l1 + p2.y * l2,
        p0.z * l0 + p1.z * l1 + p2.z * l2,
        1.0f
    );
}

__global__ void InterpolateChunkKernel(
    float4* outputPositions,
    const float4* inputPositions,
    const float* times,
    int objectCount,
    int dataFrameCount,
    int renderFrameCount,
    float startRenderTime,
    float fps
) {
    int globalId = blockIdx.x * blockDim.x + threadIdx.x;

    int totalOutput = renderFrameCount * objectCount;
    if (globalId >= totalOutput) {
        return;
    }

    int localRenderFrame = globalId / objectCount;
    int objectIndex = globalId % objectCount;

    float renderTime = startRenderTime + float(localRenderFrame) / fps;

    if (dataFrameCount <= 0) {
        return;
    }

    if (dataFrameCount == 1) {
        outputPositions[globalId] = inputPositions[objectIndex];
        return;
    }

    int right = 0;

    while (right < dataFrameCount && times[right] < renderTime) {
        ++right;
    }

    if (dataFrameCount == 2) {
        int i0 = 0;
        int i1 = 1;

        float4 p0 = inputPositions[i0 * objectCount + objectIndex];
        float4 p1 = inputPositions[i1 * objectCount + objectIndex];

        outputPositions[globalId] =
            Lerp(renderTime, times[i0], times[i1], p0, p1);

        return;
    }

    int i0;
    int i1;
    int i2;

    if (right <= 1) {
        i0 = 0;
        i1 = 1;
        i2 = 2;
    }
    else if (right >= dataFrameCount - 1) {
        i0 = dataFrameCount - 3;
        i1 = dataFrameCount - 2;
        i2 = dataFrameCount - 1;
    }
    else {
        i0 = right - 1;
        i1 = right;
        i2 = right + 1;
    }

    float4 p0 = inputPositions[i0 * objectCount + objectIndex];
    float4 p1 = inputPositions[i1 * objectCount + objectIndex];
    float4 p2 = inputPositions[i2 * objectCount + objectIndex];

    outputPositions[globalId] = QuadraticInterpolate(
        renderTime,
        times[i0],
        times[i1],
        times[i2],
        p0,
        p1,
        p2
    );
}

void UpdateChunkDataCuda(cudaGraphicsResource* cudaSSBO, DataChunkCuda chunk)
{
    if (!cudaSSBO) {
        std::cerr << "CUDA Error: cudaSSBO is null\n";
        return;
    }

    if (!chunk.times || !chunk.positions) {
        std::cerr << "CUDA Error: chunk.times or chunk.positions is null\n";
        return;
    }

    if (chunk.objectCount <= 0 ||
        chunk.dataFrameCount <= 0 ||
        chunk.renderFrameCount <= 0 ||
        chunk.fps <= 0) {
        std::cerr << "CUDA Error: invalid chunk values\n";
        return;
    }

    const int objectCount = chunk.objectCount;
    const int dataFrameCount = chunk.dataFrameCount;
    const int renderFrameCount = chunk.renderFrameCount;

    const size_t timesBytes =
        size_t(dataFrameCount) * sizeof(float);

    const size_t inputPositionsBytes =
        size_t(dataFrameCount) *
        size_t(objectCount) *
        sizeof(float4);

    const size_t outputPositionsBytes =
        size_t(renderFrameCount) *
        size_t(objectCount) *
        sizeof(float4);

    if (!EnsureCudaBuffer(
        reinterpret_cast<void**>(&gDTimes),
        &gDTimesCapacity,
        timesBytes,
        "cudaMalloc gDTimes")) {
        return;
    }

    if (!EnsureCudaBuffer(
        reinterpret_cast<void**>(&gDInputPositions),
        &gDInputPositionsCapacity,
        inputPositionsBytes,
        "cudaMalloc gDInputPositions")) {
        return;
    }

    if (!CheckCuda(
        cudaMemcpy(gDTimes, chunk.times, timesBytes, cudaMemcpyHostToDevice),
        "cudaMemcpy gDTimes")) {
        return;
    }

    if (!CheckCuda(
        cudaMemcpy(
            gDInputPositions,
            chunk.positions,
            inputPositionsBytes,
            cudaMemcpyHostToDevice
        ),
        "cudaMemcpy gDInputPositions")) {
        return;
    }

    if (!CheckCuda(
        cudaGraphicsMapResources(1, &cudaSSBO, 0),
        "cudaGraphicsMapResources")) {
        return;
    }

    float4* dOutputPositions = nullptr;
    size_t mappedSize = 0;

    if (!CheckCuda(
        cudaGraphicsResourceGetMappedPointer(
            reinterpret_cast<void**>(&dOutputPositions),
            &mappedSize,
            cudaSSBO
        ),
        "cudaGraphicsResourceGetMappedPointer")) {
        cudaGraphicsUnmapResources(1, &cudaSSBO, 0);
        return;
    }

    if (!dOutputPositions) {
        std::cerr << "CUDA Error: mapped output pointer is null\n";
        cudaGraphicsUnmapResources(1, &cudaSSBO, 0);
        return;
    }

    if (mappedSize < outputPositionsBytes) {
        std::cerr << "CUDA Error: mapped SSBO too small. mappedSize="
            << mappedSize
            << " required="
            << outputPositionsBytes
            << std::endl;

        cudaGraphicsUnmapResources(1, &cudaSSBO, 0);
        return;
    }

    int totalOutput = renderFrameCount * objectCount;
    int threads = 256;
    int blocks = (totalOutput + threads - 1) / threads;

    InterpolateChunkKernel << <blocks, threads >> > (
        dOutputPositions,
        gDInputPositions,
        gDTimes,
        objectCount,
        dataFrameCount,
        renderFrameCount,
        chunk.startRenderTime,
        float(chunk.fps)
        );

    if (!CheckCuda(cudaGetLastError(), "InterpolateChunkKernel launch")) {
        cudaGraphicsUnmapResources(1, &cudaSSBO, 0);
        return;
    }

    if (!CheckCuda(cudaDeviceSynchronize(), "InterpolateChunkKernel sync")) {
        cudaGraphicsUnmapResources(1, &cudaSSBO, 0);
        return;
    }

    CheckCuda(
        cudaGraphicsUnmapResources(1, &cudaSSBO, 0),
        "cudaGraphicsUnmapResources"
    );
}

void CleanupChunkDataCuda()
{
    if (gDTimes) {
        cudaFree(gDTimes);
        gDTimes = nullptr;
        gDTimesCapacity = 0;
    }

    if (gDInputPositions) {
        cudaFree(gDInputPositions);
        gDInputPositions = nullptr;
        gDInputPositionsCapacity = 0;
    }
}



__global__ void CopyCurrentFrameToTrailKernel(
    const float4* positionFrames,
    float4* trailHistory,
    int objectCount,
    int localRenderFrame,
    int trailWriteFrame
) {
    int objectID = blockIdx.x * blockDim.x + threadIdx.x;
    if (objectID >= objectCount) return;

    int src = localRenderFrame * objectCount + objectID;
    int dst = trailWriteFrame * objectCount + objectID;

    trailHistory[dst] = positionFrames[src];
}

bool CopyCurrentFrameToTrailCuda(
    cudaGraphicsResource* cudaPositionSSBO,
    cudaGraphicsResource* cudaTrailSSBO,
    int objectCount,
    int localRenderFrame,
    int trailWriteFrame
) {
    if (!cudaPositionSSBO || !cudaTrailSSBO) return false;
    if (objectCount <= 0) return false;
    if (localRenderFrame < 0) return false;
    if (trailWriteFrame < 0) return false;

    cudaError_t err;

    err = cudaGraphicsMapResources(1, &cudaPositionSSBO, 0);
    if (err != cudaSuccess) {
        std::cerr << "CUDA Error: map position SSBO: "
            << cudaGetErrorString(err) << std::endl;
        return false;
    }

    err = cudaGraphicsMapResources(1, &cudaTrailSSBO, 0);
    if (err != cudaSuccess) {
        std::cerr << "CUDA Error: map trail SSBO: "
            << cudaGetErrorString(err) << std::endl;
        cudaGraphicsUnmapResources(1, &cudaPositionSSBO, 0);
        return false;
    }

    size_t positionSize = 0;
    size_t trailSize = 0;

    float4* positionPtr = nullptr;
    float4* trailPtr = nullptr;

    err = cudaGraphicsResourceGetMappedPointer(
        reinterpret_cast<void**>(&positionPtr),
        &positionSize,
        cudaPositionSSBO
    );

    if (err != cudaSuccess) {
        std::cerr << "CUDA Error: get mapped position pointer: "
            << cudaGetErrorString(err) << std::endl;
        cudaGraphicsUnmapResources(1, &cudaTrailSSBO, 0);
        cudaGraphicsUnmapResources(1, &cudaPositionSSBO, 0);
        return false;
    }

    err = cudaGraphicsResourceGetMappedPointer(
        reinterpret_cast<void**>(&trailPtr),
        &trailSize,
        cudaTrailSSBO
    );

    if (err != cudaSuccess) {
        std::cerr << "CUDA Error: get mapped trail pointer: "
            << cudaGetErrorString(err) << std::endl;
        cudaGraphicsUnmapResources(1, &cudaTrailSSBO, 0);
        cudaGraphicsUnmapResources(1, &cudaPositionSSBO, 0);
        return false;
    }

    const int threads = 256;
    const int blocks = (objectCount + threads - 1) / threads;

    CopyCurrentFrameToTrailKernel << <blocks, threads >> > (
        positionPtr,
        trailPtr,
        objectCount,
        localRenderFrame,
        trailWriteFrame
        );

    err = cudaGetLastError();
    if (err != cudaSuccess) {
        std::cerr << "CUDA Error: CopyCurrentFrameToTrailKernel launch: "
            << cudaGetErrorString(err) << std::endl;
        cudaGraphicsUnmapResources(1, &cudaTrailSSBO, 0);
        cudaGraphicsUnmapResources(1, &cudaPositionSSBO, 0);
        return false;
    }

    err = cudaDeviceSynchronize();
    if (err != cudaSuccess) {
        std::cerr << "CUDA Error: CopyCurrentFrameToTrailKernel sync: "
            << cudaGetErrorString(err) << std::endl;
        cudaGraphicsUnmapResources(1, &cudaTrailSSBO, 0);
        cudaGraphicsUnmapResources(1, &cudaPositionSSBO, 0);
        return false;
    }

    cudaGraphicsUnmapResources(1, &cudaTrailSSBO, 0);
    cudaGraphicsUnmapResources(1, &cudaPositionSSBO, 0);

    return true;
}