#include "VAO.h"


VAO::VAO() {
	glGenVertexArrays(1, &ID);
}


// For GLfloat
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
	Bind();
	VBO.Bind();
	glEnableVertexAttribArray(layout);
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	VBO.Unbind();
	Unbind(); // could be bad
}
// For GLuint
void VAO::LinkIAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
	Bind();
	VBO.Bind();
	glEnableVertexAttribArray(layout);
	glVertexAttribIPointer(layout, numComponents, type, stride, offset);
	VBO.Unbind();
	Unbind();
}

void VAO::Bind() {
	glBindVertexArray(ID);
}

void VAO::Unbind() {
	glBindVertexArray(0);
}

void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}