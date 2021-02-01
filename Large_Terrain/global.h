#pragma once

#ifndef __APPLE__
#define _CRT_SECURE_NO_WARNINGS
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>
#else
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ostream>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <cassert>
#include <windows.h>
#include <process.h>
#include <set>
using namespace std;

#define VERTEXSHADERPATH "vertex.glsl"
#define FRAGMENTSHADERPATH "fragment.glsl"
#define FEEDBACKFRAGMENT "feedback_fragment.glsl"

#define TERRAINFILE "dem"

#define CHUNKNUMBER (32)
#define CHUNKSIZE (512)
#define VIEWCHUNKNUMBER (32)
#define LEVELOFTERRAIN (7)

#define CHUNKREALSIZE (5120)

#define BLENDSIZE (512)
#define LEVELOFBLENDTEX (7)

#define XMIN (15)
#define YMIN (-24)

#define LEVEL (6)
#define FINER (1)

#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Max(a, b) ((a) > (b) ? (a) : (b))

#define RIGHT (0)
#define LEFT (1)
#define BOTTOM (2)
#define TOP (3)
#define CENTER (4)
#define RIGHTBOTTOM (5)
#define RIGHTTOP (6)
#define LEFTBOTTOM (7)
#define LEFTTOP (8)

#define BORDERSIZE (4)
#define HBORDERSIZE (3)

#define sqr(x) ((x) * (x))

typedef unsigned short ushort;
typedef unsigned char  uchar;

extern const int HEIGHT, WIDTH, GRID;
extern const float fov;
