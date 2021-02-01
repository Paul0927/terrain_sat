#pragma once
#include "global.h"
#include "utils.h"
#include "basepage.h"

class Page : public BasePage {
private:
	int cellByte;
	string dataname;
	int borderSize;
	int payloadSize;

	void loadBorderData(int level, int tx, int ty, int pos);
	void loadHeightmap(int level, int tx, int ty, int pos);
	void loadHeightPage(int level, int x, int y);

public:
	Page(string dataname);
	~Page();
	virtual void loadData(int level, int x, int y);
};
