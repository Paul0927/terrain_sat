#pragma once
#include "global.h"
#include "utils.h"

class TerrainData {
private:
	//int version;
	int width, height;
	//int ntex, texnamesize;
	//int dw, dh;
	//unsigned char rt, rt2, rt3, rt4;
	//float spacing;
	unsigned short* heightmap;
	//float maxHeight, minHeight;
	//int normalW, normalH;
	//unsigned char* normal, *tangent;
	//vector<char*> texnames;
public:
	TerrainData();
	~TerrainData();
	void loadHeightmap(int level, int tx, int ty, int pos);
	int getWidth() const;
	int getHeight() const;
	unsigned short* getHeightmap() const;
	//float getMaxHeight() const;
	//float getMinHeight() const;
	void output(const char* filename);
	//void calNormal();
	//unsigned char *getNormal();
	//unsigned char *getTangent();
	//int getNormalW() const;
	//int getNormalH() const;
	void borderData(int level, int tx, int ty, int nSize, int pos);
};
