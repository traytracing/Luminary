#ifndef COMPUTE_PROGRAM_CLASS_H
#define COMPUTE_PROGRAM_CLASS_H

#include <glad/glad.h>
#include <fstream>

class ComputeProgram {
	public:
		GLuint ID;
		ComputeProgram(const char* compFile);

		void Activate();
		void Delete();
	private:
		std::string get_file_contents(const char* filename);
		void checkCompilation(GLuint shader);
		void checkLinking();
};
#endif