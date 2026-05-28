#include "Camera.h"


void Camera::UpdateMatrix(GLfloat FOVdeg, GLfloat nearPlane, GLfloat farPlane, GLfloat width, GLfloat height) {
	glm::mat4 view = glm::lookAt(position, position + orientation, up);
	glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), (GLfloat)width / height, nearPlane, farPlane);

	cameraMatrix = projection * view;
}

void Camera::UploadMatrix(RenderProgram& renderProgram, const char* uniform) {
	renderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(renderProgram.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::UploadInverseMatrix(RenderProgram& renderProgram, const char* uniform) {
	renderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(renderProgram.ID, uniform), 1, GL_FALSE, glm::value_ptr(glm::inverse(cameraMatrix)));
}


void Camera::UploadOrientation(RenderProgram& renderProgram, const char* uniform) {
	renderProgram.Activate();
	glUniform3f(glGetUniformLocation(renderProgram.ID, uniform), orientation.x, orientation.y, orientation.z);
}


void Camera::UploadPosition(RenderProgram& renderProgram, const char* uniform) {
	renderProgram.Activate();
	glUniform3f(glGetUniformLocation(renderProgram.ID, uniform), position.x, position.y, position.z);
}


void Camera::Inputs(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		speed = 0.1f;
	}


	glm::vec3 forward = orientation;
	glm::vec3 backward = -orientation;
	glm::vec3 left = -glm::normalize(glm::cross(orientation, up));
	glm::vec3 right = glm::normalize(glm::cross(orientation, up));
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += speed * forward;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position += speed * backward;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position += speed * left;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += speed * right;
	}


	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		position += speed * up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		position += speed * -up;
	}

	int Width, Height;
	glfwGetWindowSize(window, &Width, &Height);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick) {
			glfwSetCursorPos(window, Width / 2, Height / 2);
			firstClick = false;
		}

		GLdouble mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);


		GLfloat rotY = sensitivity * (GLfloat)(mouseY - (Height / 2)) / Height;

		glm::vec3 right = glm::normalize(glm::cross(up, orientation));
		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(rotY), right);

		// avoid gimbal lock
		if (!((glm::angle(newOrientation, up) <= glm::radians(5.0f)) ||
			(glm::angle(newOrientation, -up) <= glm::radians(5.0f)))) {
			orientation = newOrientation;
		}


		GLfloat rotX = sensitivity * (GLfloat)(mouseX - (Width / 2)) / Width;

		orientation = glm::rotate(orientation, glm::radians(-rotX), up);

		glfwSetCursorPos(window, Width / 2, Height / 2);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
}