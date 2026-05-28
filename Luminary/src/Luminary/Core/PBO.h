#ifndef PBO_CLASS_H
#define PBO_CLASS_H

#include <glad/glad.h>

class PBO {
	GLuint ID;
	int width = 0;
	int height = 0;

public:
	PBO();
	void AutoResize(int newWidth, int newHeight);
	void Bind();
	void Unbind();
	void Delete();


};


#endif