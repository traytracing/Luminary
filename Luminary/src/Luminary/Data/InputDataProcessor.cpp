#include "InputDataProcessor.h"
#include <iostream>
#include <algorithm>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cuda_gl_interop.h>
#include <filesystem>
#include <Luminary/Core/AssetPath.h>


namespace {
	int trailWriteFrame{ 0 };
	int previousRenderFrame{ -1 };
}

InputDataProcessor::~InputDataProcessor() {
	Reset();
}
InputDataProcessor::InputDataProcessor(const Settings& SRF) : SRF(SRF) {
}



void InputDataProcessor::SetChunkMaxData(int newMaxDataChunkByteSize) {
	lf.MaxDataChunkByteSize = newMaxDataChunkByteSize;
}

void InputDataProcessor::Reset() {
	positionSSBOC.Reset();
	trailSSBOC.Reset();

	if (fs.is_open()) {
		fs.close();
	}

	fs.clear();

	lf = LumenFile{};
	processorState = ProcessorState::NoFileLoaded;

	trailWriteFrame = 0;
	previousRenderFrame = -1;
	validTrailFrameCount = 0;
}
bool InputDataProcessor::InvalidFile() {
	Reset();
	processorState = ProcessorState::InvalidFileLoaded;
	std::cerr << "Input Data Error: " << lf.filepath.c_str() << std::endl;
	return false;
}
void InputDataProcessor::SetFile(std::string file) {
	Reset();
	lf.filepath = GetAssetPath("inputdata/" + file + ".lumen").c_str();
	processorState = UnpackFile() ? ProcessorState::FileLoaded : ProcessorState::InvalidFileLoaded;
}
bool InputDataProcessor::UnpackFile() {
	if (std::filesystem::path(lf.filepath).extension() != ".lumen") return InvalidFile();

	fs.open(lf.filepath, std::ios::in | std::ios::binary);
	if (!fs) return InvalidFile();

	fs.read(reinterpret_cast<char*> (&lf.objectCount), sizeof(int)); if (lf.objectCount <= 0) return InvalidFile();
	fs.read(reinterpret_cast<char*> (&lf.dataFrameCount), sizeof(int)); if (lf.dataFrameCount <= 0) return InvalidFile();

	lf.times.resize(lf.dataFrameCount);
	fs.read(reinterpret_cast<char*>(lf.times.data()), lf.dataFrameCount * sizeof(float)); if (lf.times.size() <= 0) return InvalidFile();

	if (!UpdateData(0)) return InvalidFile();

	processorState = ProcessorState::FileLoaded;
	return true;
}
 

bool InputDataProcessor::UpdateDataChunk(int renderFrame, bool& chunkReloaded) {
	chunkReloaded = false;

	if (lf.chunk.fps == SRF.fps && lf.IsRenderFrameInsideChunk(renderFrame)) {
		lf.chunk.renderFrameIndex = renderFrame;
		return true;
	}

	lf.chunk.fps = SRF.fps;
	lf.InitChunk(renderFrame);
	if (!lf.IsRenderFrameInsideChunk(renderFrame) && renderFrame != 0) return false;

	chunkReloaded = true;
	std::cout << "LOADING CHUNK AT RENDERFRAME: " << renderFrame << std::endl;

	uint64_t jumpToPos = uint64_t(2 * sizeof(int)) + uint64_t(lf.dataFrameCount) * sizeof(float) + uint64_t(lf.chunk.startDataFrame) * uint64_t(lf.objectCount) * sizeof(glm::vec4);
	fs.clear();
	fs.seekg(static_cast<std::streampos>(jumpToPos), std::ios::beg);
	lf.chunk.positions.resize((lf.chunk.endDataFrame - lf.chunk.startDataFrame) * lf.objectCount);
	fs.read(reinterpret_cast<char*>(lf.chunk.positions.data()), lf.chunk.positions.size() * sizeof(glm::vec4));

	return true;
}
void InputDataProcessor::UpdatePositionSSBOC() {
	int renderFrameCountInChunk = lf.chunk.endRenderFrame - lf.chunk.startRenderFrame;
	GLsizeiptr ssboSize = GLsizeiptr(renderFrameCountInChunk * lf.objectCount * sizeof(glm::vec4));
	positionSSBOC.Resize(ssboSize);
	UpdateChunkDataCuda(positionSSBOC.cudassbo, lf.makeDataChunkCuda());


	for (const auto& program : programs) {
		program->Activate();
		glUniform1i(glGetUniformLocation(program->ID, "ObjectCount"), lf.objectCount);
	}
}
void InputDataProcessor::UpdateTrailSSBOC(int renderFrame) {
	int trailFrameCount = SRF.trailTime * lf.chunk.fps;
	int localRenderFrame = lf.chunk.renderFrameIndex - lf.chunk.startRenderFrame;

	bool resized = trailSSBOC.Resize(trailFrameCount * lf.objectCount * sizeof(glm::vec4));

	bool sequential = renderFrame == previousRenderFrame + 1;
	if (!sequential) {
		trailWriteFrame = 0;
		validTrailFrameCount = 0;
	}


	trailWriteFrame = renderFrame % trailFrameCount;

	if (trailWriteFrame < 0) {
		trailWriteFrame += trailFrameCount;
	}

	bool copied = CopyCurrentFrameToTrailCuda(positionSSBOC.cudassbo, trailSSBOC.cudassbo, lf.objectCount, localRenderFrame, trailWriteFrame);


	previousRenderFrame = renderFrame;

	if (validTrailFrameCount < trailFrameCount) {
		validTrailFrameCount++;
	}

	for (const auto& program : programs) {
		program->Activate();
		glUniform1i(glGetUniformLocation(program->ID, "PositionFrameOffset"), localRenderFrame);
		glUniform1i(glGetUniformLocation(program->ID, "TrailFrameCount"), trailFrameCount);
		glUniform1i(glGetUniformLocation(program->ID, "TrailWriteFrame"), trailWriteFrame);
		glUniform1i(glGetUniformLocation(program->ID, "ValidTrailFrameCount"), validTrailFrameCount);
	}
}
bool InputDataProcessor::UpdateData(int renderFrame) {
	if (lf.chunk.fps == SRF.fps && !lf.IsTimeValidRenderFrame(renderFrame)) return false;

	// CPU data chunk
	bool chunkReloaded = false;
	if (!UpdateDataChunk(renderFrame, chunkReloaded)) return false;

	// GPU ssbo data and uniforms
	if (chunkReloaded) UpdatePositionSSBOC();
	
	UpdateTrailSSBOC(renderFrame);
	
	return true;
}



bool InputDataProcessor::AtLastRenderFrame() {
	int maxRenderFrame = static_cast<int>((lf.times.back() - lf.times.front()) * lf.chunk.fps);
	return lf.chunk.renderFrameIndex >= maxRenderFrame;
}