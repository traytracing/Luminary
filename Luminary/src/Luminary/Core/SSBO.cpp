#include "SSBO.h"
#include <iostream>

// ALWAYS DYNAMIC
SSBO::SSBO() {
	glGenBuffers(1, &ID);
}



void SSBO::SetData(void* data, GLsizeiptr size) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);

	if (size != CurrentSize) {
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		CurrentSize = size;
	}

	if (data == nullptr) {
		return;
	}


	void* ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

	if (!ptr) {
		std::cerr << "Failed to map SSBO!" << std::endl;
		return;
	}

	memcpy(ptr, data, size);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
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