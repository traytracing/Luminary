#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>

class VBO {
	public:
		VBO(GLenum Usage);
		VBO(GLfloat* data, GLsizeiptr size, GLenum Usage);

		void SetDataOnce(void* data, GLsizeiptr size);
		void SetData(void* data, GLsizeiptr size);
		void Bind();
		void Unbind();
		void Delete();

	private:
		GLuint ID;
		GLenum Usage;
		GLsizeiptr CurrentSize = 0;
		GLboolean DataSetAlready = false;
};
#endif