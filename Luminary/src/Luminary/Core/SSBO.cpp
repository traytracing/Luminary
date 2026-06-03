#include "SSBO.h"
#include <iostream>

// ALWAYS DYNAMIC
SSBO::SSBO() {
	glGenBuffers(1, &ID);
}



bool SSBO::SetData(void* data, GLsizeiptr size) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);

	bool resized = false;

	if (size > Capacity) {
		Capacity = 2 * size;
		glBufferData(GL_SHADER_STORAGE_BUFFER, Capacity, nullptr, GL_DYNAMIC_DRAW);
		resized = true;
	}

	if (data == nullptr || size == 0) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		return resized;
	}


	void* ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

	if (!ptr) {
		std::cerr << "Failed to map SSBO!" << std::endl;
		return resized;
	}

	memcpy(ptr, data, size);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	return resized;
}

void SSBO::Bind() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);
}

void SSBO::BindBase(GLuint index) {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ID);
}

void SSBO::Unbind() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::Delete() {
	glDeleteBuffers(1, &ID);
}

bool SSBO::Resize(GLsizeiptr size)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);

	bool resized = false;

	if (size > Capacity) {
		Capacity = 2 * size;
		glBufferData(GL_SHADER_STORAGE_BUFFER, Capacity, nullptr, GL_DYNAMIC_DRAW);
		resized = true;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	return resized;
}
