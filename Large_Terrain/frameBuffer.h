#pragma once
#include "global.h"
#include "shader.h"

#define SCREENVERTEXSHADERPATH "screen_vertex.glsl"
#define SCREENFRAGMENTSHADERPATH "screen_fragment.glsl"

class FrameBuffer {
private:
	GLuint frameBuffer;
	GLuint colorBuffer;
	GLuint rbo;
	bool valid;
	Shader shader;
	GLuint quadVBO;
	GLuint quadVAO;

public:
	FrameBuffer();
	~FrameBuffer();
	void init(int screenWidth, int screenHeight);
	void begin();
	void end();
	void render();
	void clear();
};

const GLfloat quadVertices[] = {   
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f, 0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f, 1.0f, 1.0f
};

