#include "utils.h"

void setUniformi(const char* name, GLint val, Shader &shader) {
	GLint loc = glGetUniformLocation(shader.Program, name);
	/*if (loc == -1) {
		cerr << "Variable " << name << " in shader not found" << endl;
	}*/
	glUniform1i(loc, val);
}

void setUniform4f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2, 
	GLfloat v3, Shader &shader) {
	GLint loc = glGetUniformLocation(shader.Program, name);
	/*if (loc == -1) {
		printf("Variable %s in shader not found\n", name);
	}*/
	glUniform4f(loc, v0, v1, v2, v3);
}

void setUniform2f(const char * name, GLfloat v0, GLfloat v1, Shader & shader) {
	GLint loc = glGetUniformLocation(shader.Program, name);
	/*if (loc == -1) {
		printf("Variable %s in shader not found\n", name);
	}*/
	glUniform2f(loc, v0, v1);
}

void setUniformMatrix4f(const char* name, glm::mat4 &matrix, Shader &shader) {
	GLint loc = glGetUniformLocation(shader.Program, name);
	if (loc == -1) {
		printf("Variable %s in shader not found\n", name);
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(matrix));
}

