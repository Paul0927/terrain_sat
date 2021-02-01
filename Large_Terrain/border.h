#pragma once

#include "global.h"

class Border {
public:
	GLuint bVAO[LEVEL+FINER][LEVEL+FINER], bVBO[LEVEL+FINER][LEVEL+FINER];
	vector<GLfloat> bVertices[FINER + LEVEL][LEVEL+FINER];

	void init();
	void draw(int level1, int level2, bool linemode);
};

