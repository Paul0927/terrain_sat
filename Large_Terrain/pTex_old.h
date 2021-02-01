//Physical Pages
#pragma once

#include "global.h"
#include "page.h"
#include <map>
#include <list>
#include "terrainData.h"
#include "ddsloader.h"
#include "sat.h"
using namespace std;

const int unused = LEVELOFBLENDTEX;

class PTex {
private:
	list<Page *> pageList[LEVELOFBLENDTEX + 1];
	int pageSize, physicalWidth, pixelSize;
	int borderSize, payloadSize;
	vector<Page*> pages;
	GLuint tex, normalTex, tangentTex;
	unsigned char *data;
	map<pair<int, pair<int, int> >, Page*> vPagepos;
	int channels;
	bool isHeightmap;
	string dataname;
	void calc_para(int i, int &xoff, int &yoff, int px, int py);

public:
	PTex(int pageSize, int phsicalWidth);
	~PTex();
	void init(bool isHeightmap, string dataname);
	Page* getPage(int index) const;
	Page* getPage(int x, int y) const;
	Page* getReplacePage();
	void insert(int level, int x, int y, Page* p);
	void clearUsed();
	GLuint getTex();
	Page* findPage(int level, int x, int y);
	unsigned char *getData();
	int getChannels() const;
	void update(int level, int x, int y, int px, int py, int vw, int vh);
	int getPixelSize() const;
	int getPhysicalWidth() const;
	int getPayloadSize() const;
	int getBorderSize() const;
	int getNormalTex() const;
	int getTangentTex() const;
};
