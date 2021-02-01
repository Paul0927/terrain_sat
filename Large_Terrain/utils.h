#pragma once
#include "global.h"
#include "shader.h"


void setUniform4f(const char* name, GLfloat v0, GLfloat v1, GLfloat v2,
	GLfloat v3, Shader &shader);

void setUniform2f(const char* name, GLfloat v0, GLfloat v1, Shader &shader);

void setUniformi(const char* name, GLint val, Shader &shader);

void setUniformMatrix4f(const char* name, glm::mat4 &matrix, Shader &shader);

inline string getChunkName(int x, int y) {
	x += XMIN;
	y += YMIN;
	char s[20];
	string ret("");
	string tmp;
	sprintf(s, "%04x", x);
	tmp = s;
	if (x < 0)
		ret += tmp.substr(4, 4);
	else
		ret += tmp;
	sprintf(s, "%04x", y);
	tmp = s;
	if (y < 0)
		ret += tmp.substr(4, 4);
	else
		ret += tmp;
	return ret;
}

inline void clamp(int &x, int l, int r) {
	//assert(l <= r);
	x = (x < l) ? l : x;
	x = (x > r) ? r : x;
}

inline void openfile(const char* filename, FILE* &fp) {
	if ((fp = fopen(filename, "rb")) == nullptr) {
		printf("Cannot open file.\n");
		//exit(1);
	}
}

inline void seekfile(FILE* fp, int off, int pos) {
	assert(pos >= 0 && pos <= 2);
	if (fseek(fp, off, 0)) {
		printf("file seek failed.\n");
		exit(1);
	}
}

inline void clamp(float & x, float l, float r) {
	if (x < l)
		x = l;
	if (x > r)
		x = r;
}

