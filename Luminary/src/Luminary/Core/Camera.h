#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "RenderProgram.h"
#include "Luminary/Settings.h"

class Camera {
	Camera(const Camera&) = default;
	Camera& operator=(const Camera&) = default;
	Camera(Camera&&) = default;
	Camera& operator=(Camera&&) = default;
public: 
	~Camera() = default;
	Camera(const Settings& SRF);
public:
	const Settings& SRF;

	GLfloat speed = 0.1f;
	GLfloat sensitivity = 100.0f;
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };

	glm::vec3 position{};
	glm::vec3 orientation{ 0.0f, 0.0f, -1.0f };
	glm::mat4 cameraMatrix{};

	void UpdateMatrix(GLfloat FOVdeg, GLfloat nearPlane, GLfloat farPlane);
	void Inputs(GLFWwindow* window);
		
	void UploadMatrix(RenderProgram& renderProgram, const char* uniform);
	void UploadOrientation(RenderProgram& renderProgram, const char* uniform);
	void UploadPosition(RenderProgram& renderProgram, const char* uniform);
	void UploadInverseMatrix(RenderProgram& renderProgram, const char* uniform);
};