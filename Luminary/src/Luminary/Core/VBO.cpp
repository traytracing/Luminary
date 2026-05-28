#include "VBO.h"
#include <iostream>


VBO::VBO(GLenum Usage) : Usage(Usage) {
	glGenBuffers(1, &ID);
}

//update * vertices
VBO::VBO(GLfloat* vertices, GLsizeiptr size, GLenum Usage) : Usage(Usage) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, Usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	DataSetAlready = true;
}

void VBO::SetDataOnce(void* data, GLsizeiptr size) {
	if (DataSetAlready) std::cerr << "Static VBO Set Twice" << std::endl;
	
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, data, Usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	DataSetAlready = true;
}

void VBO::SetData(void* data, GLsizeiptr size) {
	if (Usage != GL_DYNAMIC_DRAW) {
		std::cout << "WRONG VBO TYPE: " << Usage << std::endl;
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER, ID);
	
	if (size != CurrentSize) {
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, Usage);
		CurrentSize = size;
	}
	
	
	
	void* ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	if (!ptr) {
		std::cerr << "Failed to map VBO!" << std::endl;
		return;
		
	}

	memcpy(ptr, data, size);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete() {
	glDeleteBuffers(1, &ID);
}