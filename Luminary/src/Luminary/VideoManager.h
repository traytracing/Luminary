#pragma once

#include <cstdio>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Core/PBO.h"

#include "Settings.h"
#include "Core/AssetPath.h"

class VideoManager {
public:
	~VideoManager() = default;
	VideoManager(const Settings& SRF);

	void UpdateOutputFile(const std::string filename);
	void Start();
	void AppendFrame();
	void End();
private:
	const Settings& SRF;

	std::string outputPath;
	FILE* ffmpegPipe{ nullptr };
	const int bytesPerPixel = 4; // BGRA8
	const GLsizeiptr frameBytes = GLsizeiptr(SRF.w_Dimensions.x * SRF.w_Dimensions.y * bytesPerPixel);
	PBO ffmpegPBO;
};