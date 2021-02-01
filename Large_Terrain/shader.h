#pragma once
#include "global.h"

class Shader {
public:
	GLuint Program;
	vector<GLint> handles;
	Shader();
	void begin();
	void end();
	void attach(int type, const GLchar* filename);
	void link();
};