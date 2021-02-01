#pragma once
#include "global.h"

class TreeNode {
public:
    float cX;
    float cY;
    float size;
    bool vis;
	int level;
	float dis;
	glm::vec4 color;
	int lx, ly, rx, ry;
	TreeNode();
};
