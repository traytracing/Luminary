#include "InputManager.h"
#include <iostream>
#include <filesystem>
//fionmish later
InputManager::~InputManager() {

}

InputManager::InputManager(Camera& POVCamera, Camera& SkyCamera) : POVCamera(POVCamera), SkyCamera(SkyCamera) {
	static bool initialized = false;
	if (initialized) throw std::runtime_error("InputManager already initialized");
	initialized = true;
}



void InputManager::window_size_callback(GLFWwindow* window, int width, int height) {
	InputManager* self = static_cast<InputManager*>(glfwGetWindowUserPointer(window));


}

//change later
int curView = 6;

// change zoom later
void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	InputManager* self = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
	
	if (yoffset < 0) {
		if(curView > 0) curView--;
	}
	else {
		if(curView < 59) curView++;
	}
	self->zoom += -yoffset * 0.05f; // zoom speed 
	if (self->zoom < 0.1f) self->zoom = 0.1f;

	//std::cout << self->zoom << std::endl;
}

/// <summary>
///  change all later >
/// </summary>
GLfloat realTime = 0.0f;
GLfloat deltaRealTime = 0.0f;
GLfloat lastRealTime = 0.0f;
extern int frameIndex;

void InputManager::Update(GLFWwindow* window) {
	// REVIEW LATER:
	realTime = glfwGetTime();
	deltaRealTime = realTime - lastRealTime;
	lastRealTime = realTime;

	

	static bool firstFrame = true;
	if (firstFrame) {
		firstFrame = false;

		std::cout << "\n\n\n"        // reserve 3 lines
			<< "\x1b[3A"       // move up 3 lines
			<< "\x1b[s"        // save cursor pos here
			<< std::flush;
	}

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	float fps = 1.0f / deltaRealTime;

	// set up a consistent formatting state
	std::cout << std::fixed << std::setprecision(2);

	// every frame:
	std::cout
		<< "\x1b[u"                       // restore cursor to start of block
		// line 1
		<< "Window size: " << std::setw(4) << width << " x "
		<< std::setw(4) << height << "\x1b[K\n"
		// line 2
		<< "Frame: " << std::setw(8) << frameIndex
		<< " | FPS:  " << std::setw(8) << fps << "\x1b[K\n"
		// line 3
		<< "Time:  " << std::setw(8) << realTime
		<< " | View: " << std::setw(8) << curView << "\x1b[K"
		<< std::flush;
	

	UpdateCameras(window);
}

extern 	std::vector<std::pair<std::vector<glm::vec3>, GLfloat>> i_objectsPositionsAcrossTime;

extern int maxFrames;

// change later to be inside camera class
void InputManager::UpdateCameras(GLFWwindow* window) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	
	// Camera 1
	
	// position stuff
	POVCamera.cameraLock = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS ? false : true;

	POVCamera.speed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ?  0.4f : 0.025f;


	glm::vec3 forward = POVCamera.orientation;
	glm::vec3 backward = -POVCamera.orientation;
	glm::vec3 left = -glm::normalize(glm::cross(POVCamera.orientation, POVCamera.up));
	glm::vec3 right = glm::normalize(glm::cross(POVCamera.orientation, POVCamera.up));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) POVCamera.position += POVCamera.speed * forward;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) POVCamera.position += POVCamera.speed * backward;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) POVCamera.position += POVCamera.speed * left;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) POVCamera.position += POVCamera.speed * right;
	
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) POVCamera.position += POVCamera.speed * POVCamera.up;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) POVCamera.position += POVCamera.speed * -POVCamera.up;
	
	
	if (POVCamera.cameraLock) {
		POVCamera.orientation = -1.0f * glm::normalize(POVCamera.position);
		if (POVCamera.position == glm::vec3(0)) POVCamera.orientation = glm::vec3(1); // QUICK FIX FIX LATER
		//orientation = -1.0f * glm::normalize(glm::vec3(0, 1, 1));
		//orientation =  glm::normalize(lookat - object);
		if (frameIndex < 0 || frameIndex > maxFrames) {
			POVCamera.object = glm::vec3(1); // should be orientaiton, not object
		}
		else {
			POVCamera.object = i_objectsPositionsAcrossTime[frameIndex].first[curView];
			if (std::isnan(POVCamera.object.x))
				POVCamera.object = glm::vec3(1);
		}
		//bad fixessdjfiosdjfoisdmf move ts
		POVCamera.position = POVCamera.object + -10.0f * POVCamera.orientation;
		//orientation = lookat;
		//QUICKF IX
		
	}
	
	// orientation stuff
	if (!POVCamera.cameraLock) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (POVCamera.firstClick) {
			glfwSetCursorPos(window, width / 2, height / 2);
			POVCamera.firstClick = false;
		}

		GLdouble mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);


		GLfloat rotY = POVCamera.sensitivity * (GLfloat)(mouseY - (height / 2)) / height;

		glm::vec3 right = glm::normalize(glm::cross(POVCamera.up, POVCamera.orientation));
		glm::vec3 newOrientation = glm::rotate(POVCamera.orientation, glm::radians(rotY), right);

		// avoid gimbal lock
		if (!((glm::angle(newOrientation, POVCamera.up) <= glm::radians(5.0f)) ||
			(glm::angle(newOrientation, -POVCamera.up) <= glm::radians(5.0f)))) {
			POVCamera.orientation = newOrientation;
		}


		GLfloat rotX = POVCamera.sensitivity * (GLfloat)(mouseX - (width / 2)) / width;

		POVCamera.orientation = glm::rotate(POVCamera.orientation, glm::radians(-rotX), POVCamera.up);

		glfwSetCursorPos(window, width / 2, height / 2);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		POVCamera.firstClick = true;
	}
	



	POVCamera.UpdateMatrix(45.0f, 0.1f, 100000.0f, width, height);

	// Camera 2
	SkyCamera.orientation = POVCamera.orientation;
	SkyCamera.UpdateMatrix(45.0f, 0.1f, 100000.0f, width, height);
}

/*

double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);



	isPanning = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
	if (isPanning) {
		double dx = mouseX - lastMouseX;
		double dy = mouseY - lastMouseY;

		panOffsetX += dx * zoom;
		panOffsetY -= dy * zoom;

	}

	lastMouseX = mouseX;

	lastMouseY = mouseY;
*/