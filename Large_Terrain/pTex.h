#pragma once

#include "global.h"
#include "page.h"
#include <map>
#include <list>
#include "terrainData.h"
#include "ddsloader.h"
#include "sat.h"
#include "baselist.h"
using namespace std;

class PTex : public BaseList {
private:
    int pageSize, pixelSize, physicalWidth;
    //int level, virtualWidth;
    int borderSize, payloadSize;
	string dataname;
    GLuint tex;
	//BasePage *head, *tail;
 //   vector<vector<BasePage *> >pagePos;
    int count;

public:
    PTex(int pageSize, int physicalWidth, string dataname);
	void init(int level, int virtualWidth);
	//void moveFront(BasePage * p);
	//void insert(int level, int x, int y, BasePage * p);
	//BasePage * getReplacePage();
	//BasePage * findPage(int level, int x, int y);
	void calc_para(int i, int & xoff, int & yoff, int px, int py);
	void update(int level, int x, int y, BasePage* tp);
	int getPixelSize() const;
	int getPhysicalWidth() const;
	int getPayloadSize() const;
	int getBorderSize() const;
	GLuint getTex();
	//void clearCount();
};

class PageInfo {
	int level, x, y;
};

