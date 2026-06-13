#pragma once

#include <vector_types.h>

struct cudaGraphicsResource;

struct DataChunkCuda {
    int objectCount{};
    int dataFrameCount{};
    int renderFrameCount{};

    int fps{};
    float startRenderTime{};

    const float* times{};
    const float4* positions{};
};

void CleanupChunkDataCuda();
void UpdateChunkDataCuda(cudaGraphicsResource* cudaSSBO, const DataChunkCuda& chunk);

bool CopyCurrentFrameToTrailCuda(cudaGraphicsResource* cudaPositionSSBO, cudaGraphicsResource* cudaTrailSSBO, int objectCount, int localRenderFrame, int trailWriteFrame);
bool ClearTrailHistoryCuda(cudaGraphicsResource* cudaTrailSSBO, int objectCount, int trailFrameCount);