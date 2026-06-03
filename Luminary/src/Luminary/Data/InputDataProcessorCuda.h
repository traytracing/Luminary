#pragma once

#include <vector_types.h> // gives float4 without OpenGL headers

struct cudaGraphicsResource;

struct DataChunkCuda {
    int objectCount{};
    int dataFrameCount{}; // chunk.endDataFrame - chunk.startDataFrame

    const float* times{};
    const float4* positions{};

    int fps{};

    int startRenderFrame{};
    int renderFrameCount{};

    float startRenderTime{};
};

void CleanupChunkDataCuda();
void UpdateChunkDataCuda(cudaGraphicsResource* cudaSSBO, DataChunkCuda chunk);