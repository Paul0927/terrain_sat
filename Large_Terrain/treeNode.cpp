#include "treeNode.h"

TreeNode::TreeNode() {
	vis = true;
	color.x = (rand() % 255 + 1) / 255.0f;
	color.y = (rand() % 255 + 1) / 255.0f;
	color.z = (rand() % 255 + 1) / 255.0f;
	color.w = 1;
}
