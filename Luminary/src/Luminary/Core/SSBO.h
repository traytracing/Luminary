#pragma once

#include <glad/glad.h>

class SSBO {
public:
	~SSBO();
	SSBO();

	GLuint GetID() { return ID; }
	bool SetData(void* data, GLsizeiptr size);
	void Bind();
	void BindBase(GLuint index);
	void Unbind();
	void Delete();
	virtual bool Resize(GLsizeiptr size);
protected:
	GLuint ID;
	GLsizeiptr Capacity = 0;
};