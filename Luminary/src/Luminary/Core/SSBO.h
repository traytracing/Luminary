#ifndef SSBO_CLASS_H
#define SSBO_CLASS_H

#include <glad/glad.h>

class SSBO {
public:
	SSBO();

	GLuint GetID() { return ID; }
	bool SetData(void* data, GLsizeiptr size);
	void Bind();
	void BindBase(GLuint index);
	void Unbind();
	void Delete();
	bool Resize(GLsizeiptr size);

private:
	GLuint ID;
	GLsizeiptr Capacity = 0;
};
#endif