#include "page.h"


Page::Page(string dataname) {
	level = -1;
	prev = next = nullptr;
	data = nullptr;
	this->dataname = dataname;
	if (dataname[0] == 'd') {
		width = height = CHUNKSIZE + 2 * HBORDERSIZE;
		data = new ushort[width * height];
		cellByte = sizeof(ushort);
		borderSize = HBORDERSIZE;
		payloadSize = CHUNKSIZE;
	} else {
		width = height = BLENDSIZE + 2 * BORDERSIZE;
		data = new uchar[width * height * 3];
		cellByte = sizeof(uchar) * 3;
		borderSize = BORDERSIZE;
		payloadSize = BLENDSIZE;
	}
}

Page::~Page() {
	if (data)
		delete data;
}

void Page::loadData(int level, int x, int y) {
	if (dataname[0] == 'd') {
		//memset(heightmap, 0, width * height * 2);
		loadHeightPage(level, x, y);
	}
	else {
		loadHeightPage(level, x, y);
	}
}

void Page::loadBorderData(int level, int tx, int ty, int pos) {
	int nSize = borderSize;
	int rp;
	rp = borderSize + (payloadSize >> level) * (1 << level);

	string filename;

	int offset = (dataname[0] == 'd') ? 12 : 0;

	uchar *hp = nullptr;
	if (pos > CENTER) {
		switch (pos) {
		case LEFTTOP:
			filename = "data/" + getChunkName(tx << level, ty << level) + "/" + dataname;
			break;
		case RIGHTTOP:
			if ((tx << level) + (1 << level) - 1 >= CHUNKNUMBER)
				return;
			filename = "data/" + getChunkName((tx << level) + (1 << level) - 1, ty << level) + "/" + dataname;
			break;
		case LEFTBOTTOM:
			if ((ty << level) + (1 << level) - 1 >= CHUNKNUMBER)
				return;
			filename = "data/" + getChunkName(tx << level, (ty << level) + (1 << level) - 1) + "/" + dataname;
			break;
		case RIGHTBOTTOM:
			if ((tx << level) + (1 << level) - 1 >= CHUNKNUMBER || (ty << level) + (1 << level) - 1 >= CHUNKNUMBER)
				return;
			filename = "data/" + getChunkName((tx << level) + (1 << level) - 1, (ty << level) + (1 << level) - 1) + "/" + dataname;
			break;
		}
		filename += '0' + level;
		FILE *f;
		openfile(filename.c_str(), f);	
		int w, h;
		if (dataname[0] == 'd') {
			fread(&w, 4, 1, f);
			fread(&h, 4, 1, f);
		}
		else {
			w = h = BLENDSIZE >> level;
		}

		for (int i = 0; i < nSize; i++) {
			switch (pos) {
			case LEFTTOP:
				seekfile(f, offset + i * w * cellByte, SEEK_SET);
				//hp = heightmap + (i + HBORDERSIZE + CHUNKSIZE + 1) * width
				//	- HBORDERSIZE;
				hp = (uchar*)data + ((i + rp) * width + rp) * cellByte;
				break;
			case RIGHTTOP:
				seekfile(f, offset + ((i + 1) * w - nSize) * cellByte, SEEK_SET);
				//hp = heightmap + (i + HBORDERSIZE + CHUNKSIZE) * width;
				hp = (uchar*)data + ((i + rp) * width) * cellByte;
				break;
			case LEFTBOTTOM:
				seekfile(f, offset + (h - nSize + i) * w * cellByte, SEEK_SET);
				//hp = heightmap + (i + 1) * width - HBORDERSIZE;
				hp = (uchar*)data + (i * width + rp) * cellByte;
				break;
			case RIGHTBOTTOM:
				seekfile(f, offset + ((h - nSize + i + 1) * w - nSize) * cellByte,
					SEEK_SET);
				hp = (uchar*)data + i * width * cellByte;
				break;
			}
			fread(hp, nSize * cellByte, 1, f);
		}
		fclose(f);
	}
	else {
		if (pos == TOP || pos == BOTTOM) {
			int sX = tx << level;
			int eX = sX + (1 << level);
			int y;
			if (pos == TOP)
				y = ty << level;
			else
				y = (ty << level) + (1 << level) - 1;
			for (int x = sX; x < eX; x++) {
				filename = "data/" + getChunkName(x, y) + "/" + dataname;
				filename += '0' + level;
				FILE *f;
				openfile(filename.c_str(), f);
				int w, h;
				if (dataname[0] == 'd') {
					fread(&w, 4, 1, f);
					fread(&h, 4, 1, f);
				}
				else {
					w = h = BLENDSIZE >> level;
				}
				assert(h >= nSize);
				for (int i = 0; i < nSize; i++) {
					if (pos == TOP) {
						//hp = heightmap + (i + HBORDERSIZE + CHUNKSIZE) * width +
							//HBORDERSIZE + (x - sX) * w;
						hp = (uchar*)data + ((i + rp) * width +	borderSize + (x - sX) * w) * cellByte;
						seekfile(f, offset + i * w * cellByte, SEEK_SET);
						fread(hp, w * cellByte, 1, f);
					} 
					else {
						hp = (uchar*) data + (i * width + borderSize + (x - sX) * w) * cellByte;
						seekfile(f, offset + (h - nSize + i) * w * cellByte, SEEK_SET);
						fread(hp, w * cellByte, 1, f);
					}
				}
				fclose(f);
			}
		}
		else {
			int sY = ty << level;
			int eY = sY + (1 << level);
			int x;
			if (pos == LEFT)
				x = tx << level;
			else
				x = (tx << level) + (1 << level) -1;
			for (int y = sY; y < eY; y++) {
				filename = "data/" + getChunkName(x, y) + "/" + dataname;
				filename += '0' + level;
				FILE *f;
				openfile(filename.c_str(), f);
				int w, h;
				if (dataname[0] == 'd') {
					fread(&w, 4, 1, f);
					fread(&h, 4, 1, f);
					assert(w >= nSize);
				}
				else
					w = h = BLENDSIZE >> level;
				for (int i = 0; i < h; i++) {
					if (pos == LEFT) {
						hp = (uchar*)data + (((y - sY) * h + i + borderSize) * width + rp) * cellByte;
						seekfile(f, offset + i * w * cellByte, SEEK_SET);
						fread(hp, nSize * cellByte, 1, f);
					}
					else {
						hp = (uchar*)data + ((y - sY) * h + i + borderSize) * width * cellByte;
						seekfile(f, offset + ((i + 1) * w - nSize) * cellByte, SEEK_SET);
						fread(hp, nSize * cellByte, 1, f);
					}
				}
				fclose(f);
			}
		}
	}
}

void Page::loadHeightmap(int level, int tx, int ty, int pos) {
	if (pos != CENTER) {
		loadBorderData(level, tx, ty, pos);
		return;
	}

	int sX, eX, sY, eY;
	sX = tx << level;
	eX = (tx << level) + (1 << level);
	clamp(eX, 0, CHUNKNUMBER);
	sY = ty << level;
	eY = (ty << level) + (1 << level);
	clamp(eY, 0, CHUNKNUMBER);

	uchar *hp = nullptr;
	for (int y = sY; y < eY; y++) 
		for (int x = sX; x < eX; x++) {
			string filename = "data/" + getChunkName(x, y) + "/" + dataname;
			filename += '0' + level;
			FILE *f;
			openfile(filename.c_str(), f);

			int w, h;
			if (dataname[0] == 'd') {
				fread(&w, 4, 1, f);
				fread(&h, 4, 1, f);
				float tmp;
				fread(&tmp, sizeof(float), 1, f);
			}
			else
				w = h = BLENDSIZE >> level;


			for (int i = 0; i < h; i++) {
				hp = (uchar*)data + (((y - sY) * h + borderSize + i) * width +
					((x - sX) * w + borderSize)) * cellByte;
				fread(hp, w * cellByte, 1, f);
			}
			fclose(f);
		}
}

const int dx[9] = { -1, 1,  0, 0, 0, -1, -1,  1, 1 };
const int dy[9] = {  0, 0, -1, 1, 0, -1,  1, -1, 1 };


void Page::loadHeightPage(int level, int x, int y) {	
	int vw, vh;
	vw = CHUNKNUMBER >> level;
	vw = (vw == 0) ? 1 : vw;
	vh = vw;

	int tx, ty;
	for (int i = 0; i < 9; i++) {
		tx = x + dx[i];
		ty = y + dy[i];

		if (tx < 0 || ty < 0 || tx >= vw || ty >= vh)
			continue;
		loadHeightmap(level, tx, ty, i);
		
	}
}
//void Page::loadPage(int level, int x, int y) {
//
//}
