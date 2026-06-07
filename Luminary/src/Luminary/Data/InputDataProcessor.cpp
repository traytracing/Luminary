#include "InputDataProcessor.h"
#include <iostream>
#include <algorithm>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cuda_gl_interop.h>
#include <filesystem>


InputDataProcessor::InputDataProcessor(const Settings& SRF) : SRF(SRF)
{
}
InputDataProcessor::~InputDataProcessor()
{
	Reset();
}



void InputDataProcessor::SetChunkConfig(int newMaxDataChunkByteSize)
{
	lf.MaxDataChunkByteSize = newMaxDataChunkByteSize;
}
void InputDataProcessor::SetFile(std::string file)
{
	Reset();
	lf.filepath = file + ".lumen";
	processorState = UnpackFile() ? ProcessorState::FileLoaded : ProcessorState::InvalidFileLoaded;
}
void InputDataProcessor::Reset() {
	if (cudaSSBO) {
		cudaGraphicsUnregisterResource(cudaSSBO);
		cudaSSBO = nullptr;
	}

	if (cudaTrailSSBO) {
		cudaGraphicsUnregisterResource(cudaTrailSSBO);
		cudaTrailSSBO = nullptr;
	}

	if (fs.is_open()) fs.close();
	fs.clear();

	lf = LumenFile();
	processorState = ProcessorState::NoFileLoaded;
}
bool InputDataProcessor::InvalidFile() {
	Reset();
	processorState = ProcessorState::InvalidFileLoaded;
	std::cerr << "Input Data Error: " << lf.filepath << std::endl;
	return false;
}

bool InputDataProcessor::UnpackFile() {
	if (std::filesystem::path(lf.filepath).extension() != ".lumen") return InvalidFile();

	lf.chunk.fps = SRF.fps; // CHANGE TO GLOBAL FPS 

	fs.open(lf.filepath, std::ios::in | std::ios::binary);
	if (!fs) return InvalidFile();

	fs.read(reinterpret_cast<char*> (&lf.objectCount), sizeof(int)); if (lf.objectCount <= 0) return InvalidFile();
	fs.read(reinterpret_cast<char*> (&lf.dataFrameCount), sizeof(int)); if (lf.dataFrameCount <= 0) return InvalidFile();

	lf.times.resize(lf.dataFrameCount);
	fs.read(reinterpret_cast<char*>(lf.times.data()), lf.dataFrameCount * sizeof(float)); if (lf.times.size() <= 0) return InvalidFile();

	if (!UpdatePositionData(lf.times.front())) return InvalidFile();
	processorState = ProcessorState::FileLoaded;
}
 

bool InputDataProcessor::UpdateDataChunk(int renderFrame) {
	if(lf.IsRenderFrameInsideChunk(renderFrame)) {
		lf.chunk.renderFrameIndex = renderFrame;
		return true;
	}

	lf.InitChunk(renderFrame);
	if (!lf.IsRenderFrameInsideChunk(renderFrame) && renderFrame != 0) return false;
	std::cout << "LOADING CHUNK AT RENDERFRAME: " << renderFrame << std::endl;

	uint64_t jumpToPos =
		uint64_t(2 * sizeof(int)) +
		uint64_t(lf.dataFrameCount) * sizeof(float) +
		uint64_t(lf.chunk.startDataFrame) * uint64_t(lf.objectCount) * sizeof(glm::vec4);
	fs.clear();
	fs.seekg(static_cast<std::streampos>(jumpToPos), std::ios::beg);
	lf.chunk.positions.resize((lf.chunk.endDataFrame - lf.chunk.startDataFrame) * lf.objectCount);
	fs.read(reinterpret_cast<char*>(lf.chunk.positions.data()), lf.chunk.positions.size() * sizeof(glm::vec4));

	return true;
}


bool InputDataProcessor::UpdatePositionData(int renderFrame) {
	if (!lf.IsTimeValidRenderFrame(renderFrame)) {
		return false;
	}

	if (!UpdateDataChunk(renderFrame)) {
		return false;
	}


	// GPU
	int dataFrameCountInChunk = lf.chunk.endDataFrame - lf.chunk.startDataFrame;
	int renderFrameCountInChunk = lf.chunk.endRenderFrame - lf.chunk.startRenderFrame;
	if (dataFrameCountInChunk <= 0 || renderFrameCountInChunk <= 0) return false;

	GLsizeiptr ssboSize =
		GLsizeiptr(renderFrameCountInChunk) *
		GLsizeiptr(lf.objectCount) *
		GLsizeiptr(sizeof(glm::vec4));

	bool resized = positionSSBO.Resize(ssboSize);

	if (resized || !cudaSSBO) {
		if (cudaSSBO) {
			cudaGraphicsUnregisterResource(cudaSSBO);
			cudaSSBO = nullptr;
		}

		cudaError_t regErr = cudaGraphicsGLRegisterBuffer(
			&cudaSSBO,
			positionSSBO.GetID(),
			cudaGraphicsRegisterFlagsWriteDiscard
		);

		if (regErr != cudaSuccess) {
			std::cerr << "CUDA Error: cudaGraphicsGLRegisterBuffer : "
				<< cudaGetErrorString(regErr) << std::endl;
			cudaSSBO = nullptr;
			return false;
		}
	}

	DataChunkCuda cudaChunk{};

	cudaChunk.objectCount = lf.objectCount;
	cudaChunk.dataFrameCount = dataFrameCountInChunk;

	cudaChunk.times = lf.times.data() + lf.chunk.startDataFrame;
	cudaChunk.positions = reinterpret_cast<const float4*>(lf.chunk.positions.data());

	cudaChunk.fps = lf.chunk.fps;

	cudaChunk.startRenderFrame = lf.chunk.startRenderFrame;
	cudaChunk.renderFrameCount = renderFrameCountInChunk;

	cudaChunk.startRenderTime =
		lf.times.front() +
		float(lf.chunk.startRenderFrame) / float(lf.chunk.fps);

	UpdateChunkDataCuda(cudaSSBO, cudaChunk);

	int localRenderFrame = lf.chunk.renderFrameIndex - lf.chunk.startRenderFrame;


	if (!CopyCurrentFrameToTrail(renderFrame, localRenderFrame)) {
		return false;
	}

	for (const auto& program : programs) {
		program->Activate();
		glUniform1i(
			glGetUniformLocation(program->ID, "PositionFrameOffset"),
			localRenderFrame
		);
		glUniform1i(
			glGetUniformLocation(program->ID, "ObjectCount"),
			lf.objectCount
		);


		glUniform1i(
			glGetUniformLocation(program->ID, "TrailFrameCount"),
			trailFrameCount
		);

		glUniform1i(
			glGetUniformLocation(program->ID, "TrailWriteFrame"),
			trailWriteFrame
		);

		glUniform1i(
			glGetUniformLocation(program->ID, "ValidTrailFrameCount"),
			validTrailFrameCount
		);
	}

	return true;
}

bool InputDataProcessor::EnsureTrailSSBO() {
	if (lf.objectCount <= 0 || trailFrameCount <= 0) {
		return false;
	}

	GLsizeiptr trailSize =
		GLsizeiptr(trailFrameCount) *
		GLsizeiptr(lf.objectCount) *
		GLsizeiptr(sizeof(glm::vec4));

	bool resized = trailHistorySSBO.Resize(trailSize);

	if (resized && cudaTrailSSBO) {
		cudaGraphicsUnregisterResource(cudaTrailSSBO);
		cudaTrailSSBO = nullptr;
	}

	return RegisterTrailSSBOIfNeeded();
}
bool InputDataProcessor::RegisterTrailSSBOIfNeeded() {
	if (cudaTrailSSBO) {
		return true;
	}

	cudaError_t regErr = cudaGraphicsGLRegisterBuffer(
		&cudaTrailSSBO,
		trailHistorySSBO.GetID(),
		cudaGraphicsRegisterFlagsWriteDiscard
	);

	if (regErr != cudaSuccess) {
		std::cerr << "CUDA Error: cudaGraphicsGLRegisterBuffer trailHistorySSBO: "
			<< cudaGetErrorString(regErr) << std::endl;
		cudaTrailSSBO = nullptr;
		return false;
	}

	return true;
}
void InputDataProcessor::ResetTrailHistory() {
	trailWriteFrame = 0;
	validTrailFrameCount = 0;
}

bool InputDataProcessor::CopyCurrentFrameToTrail(int renderFrame, int localRenderFrame) {
	if (!EnsureTrailSSBO()) {
		return false;
	}

	bool sequential = renderFrame == previousRenderFrame + 1;

	if (!sequential) {
		ResetTrailHistory();
	}

	trailWriteFrame = renderFrame % trailFrameCount;

	if (trailWriteFrame < 0) {
		trailWriteFrame += trailFrameCount;
	}

	bool copied = CopyCurrentFrameToTrailCuda(
		cudaSSBO,
		cudaTrailSSBO,
		lf.objectCount,
		localRenderFrame,
		trailWriteFrame
	);

	if (!copied) {
		return false;
	}

	previousRenderFrame = renderFrame;

	if (validTrailFrameCount < trailFrameCount) {
		validTrailFrameCount++;
	}

	return true;
}

//DRAW THIS
/*
bool InputDataProcessor::UpdatePositionData(int renderFrame) {
	if (!lf.IsTimeValidRenderFrame(renderFrame)) return false;

	if (!UpdateDataChunk(renderFrame)) return false;
	


	// GPU side




	int startFrame = lf.chunk.startDataFrame;

	// One past the last frame in this lf.chunk.
	// If startFrame = 0 and dataFrameCount = 416,
	// then chunkEndExclusive = 416.
	int chunkEndExclusive = startFrame + lf.chunk.dataFrameCount;

	// The last actual frame loaded into lf.chunk.positions.
	// If startFrame = 0 and dataFrameCount = 416,
	// then lastLoadedFrame = 415.
	int lastLoadedFrame = std::min(
		chunkEndExclusive - 1,
		lf.dataFrameCount - 1
	);

	// Duration covered by the frames that are actually loaded.
	float time = lf.times[lastLoadedFrame] - lf.times[startFrame];

	lf.chunk.renderFrameCount =
		static_cast<int>(std::ceil(time * lf.chunk.fps)) + 1;

	int ssbosize =
		lf.chunk.renderFrameCount * lf.objectCount * sizeof(glm::vec4);

	std::cout
		<< "Updating Chunk | "
		<< "start=" << startFrame
		<< " lastLoaded=" << lastLoadedFrame
		<< " endExclusive=" << chunkEndExclusive
		<< " dataFrames=" << lf.chunk.dataFrameCount
		<< " startTime=" << lf.times[startFrame]
		<< " lastTime=" << lf.times[lastLoadedFrame]
		<< " duration=" << time
		<< " renderFrames=" << lf.chunk.renderFrameCount
		<< std::endl;

	if (cudaSSBO) {
		cudaGraphicsUnregisterResource(cudaSSBO);
		cudaSSBO = nullptr;
	}

	positionSSBO.Resize(ssbosize);

	cudaError_t regErr = cudaGraphicsGLRegisterBuffer(
		&cudaSSBO,
		positionSSBO.GetID(),
		cudaGraphicsRegisterFlagsWriteDiscard
	);

	if (regErr != cudaSuccess) {
		std::cerr << "CUDA Error: cudaGraphicsGLRegisterBuffer : "
			<< cudaGetErrorString(regErr) << std::endl;
		cudaSSBO = nullptr;
		return;
	}
	//positionSSBO.SetData(lf.chunk.positions.data(), lf.chunk.positions.size() * sizeof(glm::vec4));

	DataChunkCuda cudaChunk{};

	cudaChunk.objectCount = lf.objectCount;
	cudaChunk.dataFrameCount = lf.dataFrameCount;
	cudaChunk.times = lf.times.data() + lf.chunk.startDataFrame;
	cudaChunk.startDataFrame = lf.chunk.startDataFrame;
	cudaChunk.dataFrameCount = lf.chunk.dataFrameCount;
	cudaChunk.positions = reinterpret_cast<const float4*>(lf.chunk.positions.data());
	cudaChunk.fps = lf.chunk.fps;
	cudaChunk.renderFrameCount = lf.chunk.renderFrameCount;

	UpdateChunkDataCuda(cudaSSBO, cudaChunk);



	////


	for (const auto& program : programs) {
		program.Activate();
		glUniform1i(glGetUniformLocation(program.ID, "PositionFrameOffset"), lf.chunk.renderFrameIndex);
	}
	return true;
}*/