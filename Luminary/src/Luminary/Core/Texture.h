#ifndef TEXTURE_STRUCT
#define TEXTURE_STRUCT

#include <glad/glad.h>

struct Texture {
	GLuint ID;
	GLuint TextureIndex;
	GLsizei Width;
	GLsizei Height;

	Texture(const char* ShapeFile, GLuint index);

	void SetTextureIndex(GLuint index);
	void BindTex();
	void UnbindTex();
	void Delete();
};

#endif