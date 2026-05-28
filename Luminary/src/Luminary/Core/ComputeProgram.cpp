#include "ComputeProgram.h"
#include <iostream>

ComputeProgram::ComputeProgram(const char* compFile) {
	std::string compCode = get_file_contents(compFile);
	const char* vertexSource = compCode.c_str();


	GLuint compShaderID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compShaderID, 1, &vertexSource, NULL);
	glCompileShader(compShaderID);
	checkCompilation(compShaderID);


	ID = glCreateProgram();
	glAttachShader(ID, compShaderID);
	glLinkProgram(ID);
	checkLinking();
	

	glDeleteShader(compShaderID);
}



void ComputeProgram::Activate() {
	glUseProgram(ID);
}

void ComputeProgram::Delete() {
	glDeleteProgram(ID);
}



std::string ComputeProgram::get_file_contents(const char* filename) {
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

void ComputeProgram::checkCompilation(GLuint shader) {
	GLint success;
	GLchar infoLog[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		std::cerr << "ERROR::SHADER_COMPILATION_ERROR\n" << infoLog << "\n";
	}
}

void ComputeProgram::checkLinking() {
	GLint success;
	GLchar infoLog[1024];
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";
	}
}