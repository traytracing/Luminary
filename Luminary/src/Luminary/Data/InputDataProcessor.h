#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <fstream>

#include "Luminary/Core/RenderProgram.h"
#include "Luminary/Core/SSBO.h"
#include "Luminary/Settings.h"
#include "InputDataProcessorCuda.h"

enum class ProcessorState {
	NoFileLoaded,
	InvalidFileLoaded,
	FileLoaded
};

struct LumenFile { // object count and frame count (and their vectors) are gauranteed to be atleast 1
	int MaxDataChunkByteSize = 9000000;

	std::string filepath;
	int objectCount{ -1 };
	int dataFrameCount{ -1 };
	std::vector<float> times;
	bool IsTimeValidRenderFrame(int renderFrame) { // render frame starts from 0
		int maxRenderFrame = static_cast<int>((times.back() - times.front()) * chunk.fps);
		return renderFrame >= 0 && renderFrame <= maxRenderFrame;
	}

	struct DataChunk { // start and end guaranteed to be atleast 1 apart
		int startDataFrame{ -1 }, endDataFrame{ -1 };
		int dataFrameIndex{ -1 };
		std::vector<glm::vec4> positions;

		int fps{ -1 }; // chunk fps takes it from global settings when chunk is loaded
		int startRenderFrame{ -1 }, endRenderFrame{ -1 }; // render frame starts from 0
		int renderFrameIndex{ -1 };
	} chunk;
	bool IsRenderFrameInsideChunk(int renderFrame) {
		return renderFrame > chunk.startRenderFrame && renderFrame < chunk.endRenderFrame - 1;
	}

	int FindFirstDataFrame(int renderFrame) {
		float renderTime = times.front() + float(renderFrame) / float(chunk.fps);

		int left = 0;
		int right = dataFrameCount - 1;

		while (left <= right) {
			int mid = left + (right - left) / 2;

			if (times[mid] <= renderTime) {
				left = mid + 1;
			}
			else {
				right = mid - 1;
			}
		}

		return right;
	}
	int RenderFrameFromTime(float time) const
	{
		return static_cast<int>((time - times.front()) * chunk.fps);
	}
	void InitChunk(int renderFrame) { // Assumes renderFrame will become the start of the chunk
		int bufferStartRenderFrame = std::max(0, renderFrame - 1);
		int firstDataFrame = FindFirstDataFrame(bufferStartRenderFrame);

		if (firstDataFrame < 0) {
			firstDataFrame = 0;
		}

		int start = firstDataFrame;
		if (start > 0) start -= 1; // quadratic interpolation one frame buffer

		int maxDataFramesInChunk = MaxDataChunkByteSize / (objectCount * int(sizeof(glm::vec4)));
		if (maxDataFramesInChunk < 3) maxDataFramesInChunk = 3;
		int end = start + maxDataFramesInChunk;

		if (end > dataFrameCount) end = dataFrameCount;

		start = std::max(0, end - maxDataFramesInChunk); // if end is hit we still need a buffer

		chunk.startDataFrame = start;
		chunk.endDataFrame = end;
		chunk.dataFrameIndex = firstDataFrame;

		chunk.startRenderFrame =bufferStartRenderFrame;
		chunk.endRenderFrame = RenderFrameFromTime(times[chunk.endDataFrame - 1]) + 1;
		chunk.renderFrameIndex = renderFrame;
	}
};

class InputDataProcessor {
public:
	InputDataProcessor(const Settings& SRF);
	~InputDataProcessor();

	void SetChunkConfig(int newMaxDataChunkByteSize);
	void SetFile(std::string filepath);

	bool UpdatePositionData(int renderFrame);

	SSBO& GetSSBORef() { return positionSSBO; }
	void AddRenderProgram(RenderProgram& program) { programs.push_back(&program); }
private:
	void Reset();
	bool InvalidFile();
	bool UnpackFile();

	bool UpdateDataChunk(int renderFrame);

	ProcessorState processorState{ ProcessorState::NoFileLoaded };
	std::fstream fs{};
	LumenFile lf{};

	SSBO positionSSBO;
	cudaGraphicsResource* cudaSSBO{ nullptr };
	std::vector<RenderProgram*> programs;
	const Settings& SRF;
};