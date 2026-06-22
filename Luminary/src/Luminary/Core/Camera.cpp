#include "Camera.h"


Camera::Camera(const Settings& SRF) : SRF(SRF) {
}


void Camera::FreeCam(GLFWwindow* window) {
	const bool allowKeyboardMove = !SRF.cameraGuiLock && !SRF.cameraRenderLock;
	const bool allowMouseLook = !SRF.cameraGuiLock && !SRF.cameraRenderLock;

	if (allowKeyboardMove) {
		glm::vec3 forward = orientation;
		glm::vec3 backward = -orientation;
		glm::vec3 left = -glm::normalize(glm::cross(orientation, up));
		glm::vec3 right = glm::normalize(glm::cross(orientation, up));

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) position += speed * forward;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) position += speed * backward;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) position += speed * left;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) position += speed * right;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) position += speed * up;
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) position += speed * -up;
	}

	if (allowMouseLook && SRF.LeftMouseDown) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glm::vec3 right = glm::normalize(glm::cross(up, orientation));
		glm::vec2 rot = sensitivity * glm::vec2(SRF.DMousePosition) / glm::vec2(SRF.w_Dimensions);

		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rot.y), right);

		if (!((glm::angle(newOrientation, up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -up) <= glm::radians(5.0f)))) {
			orientation = newOrientation;
		}

		orientation = glm::rotate(orientation, glm::radians(-rot.x), up);
		orientation = glm::normalize(orientation);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}
void Camera::ObjectCam(GLFWwindow* window) {
	const bool hasSource = SRF.objectSource != -1;
	const bool hasTarget = SRF.objectTarget != -1;

	const bool allowKeyboardMove = !SRF.cameraGuiLock && !SRF.cameraRenderLock;
	const bool allowMouseLook = !SRF.cameraGuiLock && !SRF.cameraRenderLock;

	const float followDistance = 5.0f;

	const glm::vec3 sourcePosition = glm::vec3(SRF.objectSourcePosition);
	const glm::vec3 targetPosition = glm::vec3(SRF.objectTargetPosition);

	auto ApplyMouseLook = [&]() {
		if (allowMouseLook && SRF.LeftMouseDown) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			glm::vec3 right = glm::normalize(glm::cross(up, orientation));
			glm::vec2 rot = sensitivity * glm::vec2(SRF.DMousePosition) / glm::vec2(SRF.w_Dimensions);

			glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rot.y), right);

			if (!((glm::angle(newOrientation, up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -up) <= glm::radians(5.0f)))) {
				orientation = newOrientation;
			}

			orientation = glm::rotate(orientation, glm::radians(-rot.x), up);
			orientation = glm::normalize(orientation);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		};

	if (hasSource && hasTarget) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		glm::vec3 sourceToTarget = targetPosition - sourcePosition;

		if (glm::dot(sourceToTarget, sourceToTarget) > 0.000001f) {
			orientation = glm::normalize(sourceToTarget);
		}

		position = sourcePosition - orientation * followDistance;
		return;
	}

	if (!hasSource && hasTarget) {
		ApplyMouseLook();

		if (allowKeyboardMove) {
			glm::vec3 forward = orientation;
			glm::vec3 backward = -orientation;
			glm::vec3 left = -glm::normalize(glm::cross(orientation, up));
			glm::vec3 right = glm::normalize(glm::cross(orientation, up));

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) position += speed * forward;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) position += speed * backward;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) position += speed * left;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) position += speed * right;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) position += speed * up;
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) position += speed * -up;
		}

		glm::vec3 toTarget = targetPosition - position;

		if (glm::dot(toTarget, toTarget) > 0.000001f) {
			orientation = glm::normalize(toTarget);
		}

		return;
	}

	if (hasSource && !hasTarget) {
		ApplyMouseLook();

		position = sourcePosition - orientation * followDistance;
		return;
	}

	FreeCam(window);
}
void Camera::Inputs(GLFWwindow* window) {
	if (SRF.objectSource == -1 && SRF.objectTarget == -1) {
		FreeCam(window);
	}
	else {
		ObjectCam(window);
	}
}

void Camera::UpdateMatrix(GLfloat FOVdeg, GLfloat nearPlane, GLfloat farPlane) {
	glm::mat4 view = glm::lookAt(position, position + orientation, up);
	glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), SRF.w_Dimensions.x / GLfloat(SRF.w_Dimensions.y), nearPlane, farPlane);

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