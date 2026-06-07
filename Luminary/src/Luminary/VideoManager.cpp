#include "VideoManager.h"
#include <iostream>
#include "Core/AssetPath.h"

VideoManager::VideoManager(const Settings& SRF) : SRF(SRF) {
    ffmpegPBO.Bind();
    glBufferData(GL_PIXEL_PACK_BUFFER, frameBytes, nullptr, GL_STREAM_READ);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void VideoManager::UpdateOutputFile(const std::string filename) {
    outputPath = GetAssetPath((std::filesystem::path("outputs") / (filename + ".mp4")).string());
}

void VideoManager::Start() {
    End();

    static const std::string ffmpegDir = GetAssetPath((std::filesystem::path("vendor") / "ffmpeg").string());
    static const std::string ffmpegExe = GetAssetPath((std::filesystem::path("vendor") / "ffmpeg" / "ffmpeg.exe").string());
    static const std::string outputDir = GetAssetPath(std::filesystem::path("outputs").string());
    std::filesystem::create_directories(outputDir);

    char cmd[4096];
    std::snprintf(cmd, sizeof(cmd),
        "cd /d \"%s\" && "
        "\"%s\" -loglevel info -report -y "
        "-f rawvideo -pix_fmt bgra -s %dx%d -r %d -i - "
        "-vf \"vflip\" "
        "-c:v h264_nvenc -preset p7 -tune hq "
        "-rc vbr_hq -multipass fullres -b:v 0 -cq 16 -rc-lookahead 32 "
        "-spatial_aq 1 -temporal_aq 1 -aq-strength 12 "
        "-bf 3 -b_ref_mode middle -refs 16 "
        "-profile:v high -pix_fmt yuv420p "
        "-color_primaries bt709 -color_trc bt709 -colorspace bt709 "
        "-movflags +faststart "
        "\"%s\"",
        ffmpegDir.c_str(),
        ffmpegExe.c_str(),
        SRF.w_Dimensions.x,
        SRF.w_Dimensions.y,
        SRF.fps,
        outputPath.c_str()
    );

    ffmpegPipe = _popen(cmd, "wb");

    if (!ffmpegPipe)
        throw std::runtime_error("Failed to start ffmpeg.");
}

void VideoManager::AppendFrame() {
    if (!ffmpegPipe || SRF.appState != AppStateType::Rendering) return;

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glReadBuffer(GL_BACK);

	ffmpegPBO.Bind();
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, SRF.w_Dimensions.x, SRF.w_Dimensions.y, GL_BGRA, GL_UNSIGNED_BYTE, 0);
	GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	if (ptr) {
		std::fwrite(ptr, 1, size_t(frameBytes), ffmpegPipe);
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	}

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void VideoManager::End() {
	if (!ffmpegPipe) return;

    std::fflush(ffmpegPipe);
	_pclose(ffmpegPipe);
    ffmpegPipe = nullptr;
}