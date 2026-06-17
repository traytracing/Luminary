#pragma once

#include <glad/glad.h>

class PBO {
public:
	PBO();
	void AutoResize(int newWidth, int newHeight);
	void Bind();
	void Unbind();
	void Delete();
	GLuint GetID() { return ID; }
private:
	GLuint ID;
	int width = 0;
	int height = 0;
};