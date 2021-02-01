#include "pTex.h"
#include "utils.h"

PTex::PTex(int pageSize, int physicalWidth) {
	pages.clear();
	for (int i = 0; i <= unused; i++)
		pageList[i].clear();

	this->payloadSize = pageSize;
	this->physicalWidth = physicalWidth;
	isHeightmap = false;
    data = nullptr;
}

PTex::~PTex() {
	for (int i = 0; i < pages.size(); i++)
		if (pages[i])
			delete pages[i];
	if (data)
		delete data;
}

void PTex::init(bool isHeightmap, string dataname) {
	if (!isHeightmap)
		this->borderSize = BORDERSIZE;
	else
		this->borderSize = HBORDERSIZE;
	this->pageSize = payloadSize + 2 * borderSize;
	pixelSize = this->pageSize * physicalWidth;

	for (int i = 0; i < physicalWidth; i++)
		for (int j = 0; j < physicalWidth; j++) {
			Page* t = new Page();
			t->setPos(j, i);
			t->setLevel(unused);
			pages.push_back(t);
			pageList[unused].push_back(t);
		}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (isHeightmap) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, pixelSize, pixelSize,
			0, GL_RED, GL_UNSIGNED_SHORT, nullptr);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
			pixelSize, pixelSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	if (isHeightmap) {
		//glGenTextures(1, &normalTex);
		//glBindTexture(GL_TEXTURE_2D, normalTex);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixelSize, pixelSize, 0,
		//	GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		//glBindTexture(GL_TEXTURE_2D, 0);

		//glGenTextures(1, &tangentTex);
		//glBindTexture(GL_TEXTURE_2D, tangentTex);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixelSize, pixelSize, 0,
		//	GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		//glBindTexture(GL_TEXTURE_2D, 0);
	}

	vPagepos.clear();

	this->isHeightmap = isHeightmap;
	this->dataname = dataname;
}

Page* PTex::getPage(int index) const {
	if (index < pages.size())
		return pages[index];
	else
		return nullptr;
}

Page * PTex::getPage(int x, int y) const {
	if (x >= physicalWidth || y >= physicalWidth)
		return nullptr;
	return pages[y * physicalWidth + x];
}

Page* PTex::getReplacePage() {
	for (int i = unused; i >= 0; i--)
		for (list<Page*>::iterator iter = pageList[i].begin(); iter != pageList[i].end(); iter++)
			if ((*iter)->getUsed())
				continue;
			else
				return (*iter);
	printf("The number of physical pages is too little.\n");
	exit(0);
}

void PTex::insert(int level, int x, int y, Page * p) {
	int oLevel = p->getLevel();

	pageList[oLevel].remove(p);
	pageList[level].push_back(p);

	if (oLevel != unused)
		vPagepos.erase(make_pair(oLevel, make_pair(p->getOx(), p->getOy())));

	p->setOpos(x, y);
	p->setLevel(level);
	vPagepos.insert(make_pair(make_pair(level, make_pair(x, y)), p));
}

void PTex::clearUsed() {
	for (int i = 0; i < pages.size(); i++)
		pages[i]->resetUsed();
}

GLuint PTex::getTex() {
	return tex;
}

Page * PTex::findPage(int level, int x, int y) {
	map<pair<int, pair<int, int>>, Page*>::iterator result;
	result = vPagepos.find(make_pair(level, make_pair(x, y)));
	if (result != vPagepos.end())
		return result->second;
	else
		return nullptr;
}

unsigned char * PTex::getData() {
	return data;
}

int PTex::getChannels() const {
	return channels;
}

const int dx[9] = { -1, 1,  0, 0, 0, -1, -1,  1, 1 };
const int dy[9] = {  0, 0, -1, 1, 0, -1,  1, -1, 1 };

void PTex::calc_para(int i, int &xoff, int &yoff, int px, int py) {
	switch (i) {
	case TOP:
		xoff = px * pageSize + borderSize;
		yoff = (py + 1) * pageSize - borderSize;
		break;
	case BOTTOM:
		xoff = px * pageSize + borderSize;
		yoff = py * pageSize;
		break;
	case LEFT:
		xoff = (px + 1) * pageSize - borderSize;
		yoff = py * pageSize + borderSize;
		break;
	case RIGHT:
		xoff = px * pageSize;
		yoff = py * pageSize + borderSize;
		break;
	case CENTER:
		xoff = px * pageSize + borderSize;
		yoff = py * pageSize + borderSize;
		break;
	case LEFTTOP:
		xoff = (px + 1) * pageSize - borderSize;
		yoff = (py + 1) * pageSize - borderSize;
		break;
	case RIGHTTOP:
		xoff = px * pageSize;
		yoff = (py + 1) * pageSize - borderSize;
		break;
	case LEFTBOTTOM:
		xoff = (px + 1) * pageSize - borderSize;
		yoff = py * pageSize;
		break;
	case RIGHTBOTTOM:
		xoff = px * pageSize;
		yoff = py * pageSize;
		break;
	}
}

void PTex::update(int level, int x, int y, int px, int py, int vw, int vh) {
	string filename;
	static TerrainData terrainData;
	static DDSLoader ddsloader;

	int tx, ty;
	int xoff, yoff, width, height;

	for (int i = 0; i < 9; i++) {
		tx = x + dx[i];
		ty = y + dy[i];

		if (tx < 0 || ty < 0 || tx >= vw || ty >= vh)
			continue;

		calc_para(i, xoff, yoff, px, py);
		if (isHeightmap) {
			terrainData.loadHeightmap(level, tx, ty, i);
		}
		else {
			ddsloader.loadCompressedData(level, i, tx, ty, dataname);
			glBindTexture(GL_TEXTURE_2D, tex);
			glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, xoff, yoff,
				ddsloader.getWidth(), ddsloader.getHeight(),
				GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
				ddsloader.getImageSize(),
				ddsloader.getData());
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	if (isHeightmap) {
		/*if (!x && !y)
			terrainData.output("testout.txt");*/
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, px * pageSize, py * pageSize,
			terrainData.getWidth(), terrainData.getHeight(),
			GL_RED, GL_UNSIGNED_SHORT, terrainData.getHeightmap());
		glBindTexture(GL_TEXTURE_2D, 0);
		//terrainData.calNormal();
		////if (!px && !py)
		//	//terrainData.output("testout.txt");
		//glBindTexture(GL_TEXTURE_2D, normalTex);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, px * pageSize + 1, py * pageSize + 1,
		//	terrainData.getNormalW(), terrainData.getNormalH(),
		//	GL_RGBA, GL_UNSIGNED_BYTE, terrainData.getNormal());
		//glBindTexture(GL_TEXTURE_2D, 0);
		//glBindTexture(GL_TEXTURE_2D, tangentTex);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, px * pageSize + 1, py * pageSize + 1,
		//	terrainData.getNormalW(), terrainData.getNormalH(),
		//	GL_RGBA, GL_UNSIGNED_BYTE, terrainData.getTangent());
		//glBindTexture(GL_TEXTURE_2D, 0);
	}
}

int PTex::getPixelSize() const {
	return pixelSize;
}

int PTex::getPhysicalWidth() const {
	return physicalWidth;
}

int PTex::getPayloadSize() const {
	return payloadSize;
}

int PTex::getBorderSize() const {
	return borderSize;
}

int PTex::getNormalTex() const {
	return normalTex;
}

int PTex::getTangentTex() const {
	return tangentTex;
}
