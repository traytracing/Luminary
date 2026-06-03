#include "RenderProgram.h"
#include <iostream>

RenderProgram::RenderProgram(const char* vertexFile, const char* fragmentFile) {
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();



	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexSource, NULL);
	glCompileShader(vertexShaderID);
	checkShaderCompilation(vertexShaderID, "VERTEX");

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShaderID);
	checkShaderCompilation(vertexShaderID, "FRAGMENT");

	ID = glCreateProgram();
	glAttachShader(ID, vertexShaderID);
	glAttachShader(ID, fragmentShaderID);
	glLinkProgram(ID);
	checkShaderLinking(ID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

RenderProgram::RenderProgram(const char* vertexFile, const char* geomFile, const char* fragmentFile) {
	std::string vertexCode = get_file_contents(vertexFile);
	std::string geometryCode = get_file_contents(geomFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* geometrySource = geometryCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();



	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexSource, NULL);
	glCompileShader(vertexShaderID);
	checkShaderCompilation(vertexShaderID, "VERTEX");

	GLuint geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShaderID, 1, &geometrySource, NULL);
	glCompileShader(geometryShaderID);
	checkShaderCompilation(geometryShaderID, "GEOMETRY");

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShaderID);
	checkShaderCompilation(fragmentShaderID, "FRAGMENT");

	ID = glCreateProgram();
	glAttachShader(ID, vertexShaderID);
	glAttachShader(ID, geometryShaderID);
	glAttachShader(ID, fragmentShaderID);
	glLinkProgram(ID);
	checkShaderLinking(ID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(geometryShaderID);
	glDeleteShader(fragmentShaderID);
}




void RenderProgram::Activate() const {
	glUseProgram(ID);
}

void RenderProgram::Delete() const {
	glDeleteProgram(ID);
}



std::string RenderProgram::get_file_contents(const char* filename) {
	std::ifstream in(filename, std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	throw(errno);
}

void RenderProgram::checkShaderCompilation(GLuint shader, const std::string& type) {
	GLint success;
	GLchar infoLog[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
	}
}

void RenderProgram::checkShaderLinking(GLuint program) {
	GLint success;
	GLchar infoLog[1024];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 1024, NULL, infoLog);
		std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";
	}
}