//Single Page
#pragma once

#include "global.h"
//#include "image.h"

class Page {
private:
	int size;
	int level;
	bool used;
	unsigned char *pixels;
	int x, y;
	int ox, oy;
	int channels;

public:
	//Page* next;
	//Page* prev;
	Page();
	~Page();
	void setUsed();
	bool getUsed();
	void resetUsed();
	void setPos(int x, int y);
	int getX() const;
	int getY() const;
	void setOpos(int ox, int oy);
	int getOx() const;
	int getOy() const;
	unsigned char *getPixels();
	int getChannels() const;
	void setLevel(int level);
	int getLevel() const;
};
