#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "RenderProgram.h"

class Camera {
public: 
	Camera() = default;
	~Camera() = default;

	Camera(const Camera&) = default;
	Camera& operator=(const Camera&) = default;
	Camera(Camera&&) = default;
	Camera& operator=(Camera&&) = default;

public:
	// move all of these into private after changing manager camera inputs
	GLfloat speed = 0.1f;
	GLfloat sensitivity = 100.0f;
	GLboolean firstClick = true;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	//

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);

	glm::vec3 object = glm::vec3(0.0f);
	glm::vec3 lookat = glm::vec3(0.0f);

	bool cameraLock = false; // fix later


	void UpdateMatrix(GLfloat FOVdeg, GLfloat nearPlane, GLfloat farPlane, GLfloat width, GLfloat height);
		
	void UploadMatrix(RenderProgram& renderProgram, const char* uniform);
	void UploadOrientation(RenderProgram& renderProgram, const char* uniform);
	void UploadPosition(RenderProgram& renderProgram, const char* uniform);
	void UploadInverseMatrix(RenderProgram& renderProgram, const char* uniform);

	void Inputs(GLFWwindow* window); // fix later with input manager

private:
	

};

#endif