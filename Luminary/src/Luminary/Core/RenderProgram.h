#ifndef RENDER_PROGRAM_CLASS_H
#define RENDER_PROGRAM_CLASS_H

#include <glad/glad.h>
#include <fstream>

class RenderProgram {
public:
	GLuint ID;
	RenderProgram() = default;
	RenderProgram(const char* vertexFile, const char* fragmentFile);
	RenderProgram(const char* vertexFile, const char* geomFile, const char* fragmentFile);

	void Activate() const;
	void Delete() const;
private:
	std::string get_file_contents(const char* filename);
	void checkShaderCompilation(GLuint shader, const std::string& type);
	void checkShaderLinking(GLuint program);
};
#endif