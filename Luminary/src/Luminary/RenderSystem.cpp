#include "RenderSystem.h"
#include <stb_image_write.h>
#include <thread>
#include <string>
#include "Luminary/Core/PBO.h"

int maxFrames = 0;
int totalSimTime = 30;

void RenderSystem::InputData(int timestampsCount, std::vector<double> timestamps, int objectCount, std::vector<std::vector<glm::vec3>> positions) {
	maxFrames = 165* totalSimTime -1;
	//nbodyRenderer.InterpolateData(timestampsCount, timestamps, objectCount, positions); // upgrade later
}
bool renderGrid = true;

void RenderSystem::Render() {
	//grid
	

	skyRenderer.Render();
	// camera stuff
	nbodyRenderer.Render();
	
	glEnable(GL_DEPTH_TEST);	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	// should this grid reall be here?
	gridRenderProgram.Activate();
	POVCamera.UploadPosition(gridRenderProgram, "camPos");
	POVCamera.UploadMatrix(gridRenderProgram, "MVP");

	vaoooo.Bind();
	if(renderGrid)
		glDrawArrays(GL_TRIANGLES, 0, 6);
}

int fps = 165; // fix main loop with realtime fps
int frameIndex = 0;
extern int curView;
extern glm::vec4 trailColor;
int mult = 1;
extern int randomNum;
extern GLfloat realTime;

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <cstdio>
#include <stdexcept>
#include <string>
// TODO create a new output folder when it has ran


// 1) Launch ffmpeg ready to read raw frames from stdin.
//    NOTE: change the path to where your bundled ffmpeg.exe lives.
FILE* start_ffmpeg_pipe(
	int width, int height, int fps,
	const std::string& outMp4,
	const std::string& ffmpegExe = "ffmpeg" // or "tools\\ffmpeg\\ffmpeg.exe"
) {
	char cmd[1024];
	std::snprintf(cmd, sizeof(cmd),
		"cd /d \"C:\\Temp\" && "
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
		ffmpegExe.c_str(), width, height, fps, outMp4.c_str());

	// 2) _popen runs ffmpeg and gives us a writable stream to its stdin.
	//    "wb" = write, binary (don’t translate \n).
	FILE* ffmpegPipe = _popen(cmd, "wb");
	if (!ffmpegPipe) throw std::runtime_error("Failed to start ffmpeg.");
	return ffmpegPipe;
}

// 3) When you are done sending frames, close the pipe so ffmpeg can finalize MP4.
void finish_ffmpeg_pipe(FILE* ffmpegPipe) {
	if (!ffmpegPipe) return;
	std::fflush(ffmpegPipe);
	_pclose(ffmpegPipe); // blocks until ffmpeg finishes writing trailer and exits
}


//TODO lock the resolution when rendering 
void RenderSystem::Begin() {
	int width = 1920; int height = 1080;
	//glfwSwapInterval(0);
	glReadBuffer(GL_BACK);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	// get file first then do it 
	PBO PBO1;
	PBO1.Bind();
	const int bytesPerPixel = 4; // BGRA8
	const GLsizeiptr frameBytes = GLsizeiptr(width) * height * bytesPerPixel;
	glBufferData(GL_PIXEL_PACK_BUFFER, frameBytes, nullptr, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	FILE* ffmpegPipe = start_ffmpeg_pipe(width, height, fps, "C:\\Temp\\capture.mp4"); // your function

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		static ImVec4 myColor = ImVec4(0.749, 0.659, 0.565, 1.0f);

		ImGui::Begin("Controls");
		//ImGui::Text("This is ImGui running on OpenGL 4.6!");
		float fpsim = ImGui::GetIO().Framerate;
		ImGui::Text("FPS: %.1f", fpsim);
		ImGui::Text("Seed: %d", randomNum);
		ImGui::SliderInt("View: ", &curView, 0, 29);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));      // background
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));   // handle
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 0.2f, 0.2f, 1.0f)); // active handle

		ImGui::SliderInt("FrameIndex: ", &frameIndex, 0, 15599);

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);
		ImGui::ColorEdit4("Pick a color", (float*)&myColor);
		trailColor = glm::vec4(myColor.x, myColor.y, myColor.z, myColor.w);
		static char filePath[128] = "";
		if (ImGui::InputText("File path", filePath, IM_ARRAYSIZE(filePath))) {
		}
		if (ImGui::Button("Load")) {
		}
		if (ImGui::Button("Reverse")) {
			mult *= -1;
		}
		ImGui::Checkbox("Show grid", &renderGrid);

		ImGui::End();
		ImGui::SetNextWindowSize(ImVec2(240, 100)); // width=600, height=200

		ImGui::Begin("Simulation Time", nullptr,
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::Text("%.1f", frameIndex / 165.0f);


		ImGui::End();

		inputManager.Update(window); // BIG


		glfwGetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.106f, 0.110f, 0.173f, 1.0f);
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Render();

		/*
		PBO1.Bind();
		//PBO1.AutoResize(width, height);
		// Read from backbuffer into PBO
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0); // 0 means offset into bound PBO
		stbi_flip_vertically_on_write(1);

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		//std::string filename = "outputs/movie/frame_" + std::to_string((frameIndex / 1000) % 10) + std::to_string((frameIndex / 100) % 10) + std::to_string((frameIndex / 10) % 10) + std::to_string(frameIndex % 10) + ".jpg";
		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		if (ptr) {
			//stbi_write_jpg(filename.c_str(), width, height, 4, ptr, width * 4);
			//stbi_write_png(filename.c_str(), width, height, 4, ptr, width * 4);

			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		}

		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		*/






		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		PBO1.Bind();
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		// 1) Ask GL to write pixel data into the PBO (note the null pointer!)
		glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0);

		// 2) Map the PBO to get a CPU pointer
		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		if (ptr) {
			// 3) Write exactly one frame to ffmpeg
			std::fwrite(ptr, 1, size_t(frameBytes), ffmpegPipe);

			// If the image is upside-down, either flip rows here manually
			// before fwrite (slower), or add "-vf vflip" to the ffmpeg args.
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		}

		// 4) Unbind
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		glfwSwapBuffers(window);
		frameIndex += mult;
		if (frameIndex < 0)
			frameIndex = 0;
		else if (frameIndex > maxFrames)
			frameIndex = maxFrames;
		if (realTime > 100) {
			break;
		}
		
	}
	finish_ffmpeg_pipe(ffmpegPipe);

	this->~RenderSystem();
}



RenderSystem::RenderSystem(GLFWwindow* window, GLint iWidth, GLint iHeight) : window(window), inputManager(POVCamera, SkyCamera) {
	static bool initialized = false;
	if (initialized) throw std::runtime_error("RenderSystem already initialized");
	initialized = true;

	glfwSetWindowSizeCallback(window, inputManager.window_size_callback);
	glfwSetScrollCallback(window, inputManager.scroll_callback);
	glfwSetWindowUserPointer(window, &inputManager);
}

RenderSystem::~RenderSystem()
{
	// add more
	glfwDestroyWindow(window);
}
