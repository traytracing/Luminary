#ifndef SSBO_CLASS_H
#define SSBO_CLASS_H

#include <glad/glad.h>

class SSBO {
	public:
		SSBO();

		void SetData(void* data, GLsizeiptr size);
		void Bind();
		void BindBase(GLuint index);
		void Unbind();
		void Delete();

	private:
		GLuint ID;
		GLsizeiptr CurrentSize = 0;
};
#endif