#include "terrainData.h"

TerrainData::TerrainData() {
	int s = CHUNKSIZE + 2 * HBORDERSIZE;
	width = height = s;
	heightmap = new unsigned short[width * height];

	//normalW = normalH = s - 2;
	//normal = new unsigned char[4 * normalW * normalH];
	//tangent = new unsigned char[4 * normalW * normalH];
}


TerrainData::~TerrainData() {
	if (heightmap)
		delete heightmap;
	//if (normal)
		//delete normal;
	//if (tangent)
		//delete tangent;
}

void TerrainData::loadHeightmap(int level, int tx, int ty, int pos) {
	if (pos != CENTER) {
		borderData(level, tx, ty, HBORDERSIZE, pos);
		return;
	}

	int sX, eX, sY, eY;
	sX = tx << level;
	eX = (tx << level) + (1 << level);
	sY = ty << level;
	eY = (ty << level) + (1 << level);

	unsigned short *hp = nullptr;
	for (int y = sY; y < eY; y++) 
		for (int x = sX; x < eX; x++) {
			string filename = "data/" + getChunkName(x, y) + "/" + TERRAINFILE;
			filename += '0' + level;
			FILE *f;
			openfile(filename.c_str(), f);

			int w, h;
			fread(&w, 4, 1, f);
			fread(&h, 4, 1, f);

			float tmp;
			fread(&tmp, sizeof(float), 1, f);

			for (int i = 0; i < h; i++) {
				hp = heightmap + ((y - sY) * h + HBORDERSIZE + i) * width +
					((x - sX) * w + HBORDERSIZE);
				fread(hp, w * sizeof(short), 1, f);
				//fread(heightmap + (i + HBORDERSIZE) * width + HBORDERSIZE,
				//	sizeof(float) * w, 1, f);
			}
			fclose(f);
		}

	//if (tx == 1 && ty == 10 && level == 0)
		//memset(heightmap, 0, width * height * 4);

	//memset(heightmap, 0, width * height * 4);
	//if (tx == 0 && ty == 0) {
	//	for (int i = HBORDERSIZE; i < height - HBORDERSIZE; i++)
	//		for (int j = HBORDERSIZE; j < width - HBORDERSIZE; j++)
	//			heightmap[i * width + j] = 100;
	//}
}

int TerrainData::getWidth() const {
	return width;
}

int TerrainData::getHeight() const {
	return height;
}

unsigned short* TerrainData::getHeightmap() const {
	return heightmap;
}
//
//float TerrainData::getMaxHeight() const {
//	return maxHeight;
//}
//
//float TerrainData::getMinHeight() const {
//	return minHeight;
//}

void TerrainData::output(const char * filename) {
	freopen(filename, "w", stdout);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("%d ", heightmap[i * width + j]);
		}
		printf("\n");
	}
	freopen("CON", "w", stdout);
	//SOIL_save_image(filename, SOIL_SAVE_TYPE_BMP, normalW, normalH, 3, normal);
}


void TerrainData::borderData(int level, int tx, int ty, int nSize, int pos) {
	assert(nSize == HBORDERSIZE);

	int rp;
	rp = HBORDERSIZE + (CHUNKSIZE >> level) * (1 << level);

	string filename;

	int offset = 12;

	unsigned short *hp = nullptr;
	if (pos > CENTER) {
		switch (pos) {
		case LEFTTOP:
			filename = "data/" + getChunkName(tx << level, ty << level) + "/" + TERRAINFILE;
			break;
		case RIGHTTOP:
			filename = "data/" + getChunkName((tx << level) + (1 << level) - 1, ty << level) + "/" + TERRAINFILE;
			break;
		case LEFTBOTTOM:
			filename = "data/" + getChunkName(tx << level, (ty << level) + (1 << level) - 1) + "/" + TERRAINFILE;
			break;
		case RIGHTBOTTOM:
			filename = "data/" + getChunkName((tx << level) + (1 << level) - 1, (ty << level) + (1 << level) - 1) + "/" + TERRAINFILE;
			break;
		}
		filename += '0' + level;
		FILE *f;
		openfile(filename.c_str(), f);	
		int w, h;
		fread(&w, 4, 1, f);
		fread(&h, 4, 1, f);

		for (int i = 0; i < nSize; i++) {
			switch (pos) {
			case LEFTTOP:
				seekfile(f, offset + i * w * sizeof(short), SEEK_SET);
				//hp = heightmap + (i + HBORDERSIZE + CHUNKSIZE + 1) * width
				//	- HBORDERSIZE;
				hp = heightmap + (i + rp) * width + rp;
				break;
			case RIGHTTOP:
				seekfile(f, offset + ((i + 1) * w - nSize) * sizeof(short), SEEK_SET);
				//hp = heightmap + (i + HBORDERSIZE + CHUNKSIZE) * width;
				hp = heightmap + (i + rp) * width;
				break;
			case LEFTBOTTOM:
				seekfile(f, offset + (h - nSize + i) * w * sizeof(short), SEEK_SET);
				//hp = heightmap + (i + 1) * width - HBORDERSIZE;
				hp = heightmap + i * width + rp;
				break;
			case RIGHTBOTTOM:
				seekfile(f, offset + ((h - nSize + i + 1) * w - nSize) * sizeof(short),
					SEEK_SET);
				hp = heightmap + i * width;
				break;
			}
			fread(hp, nSize * sizeof(short), 1, f);
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
				filename = "data/" + getChunkName(x, y) + "/" + TERRAINFILE;
				filename += '0' + level;
				FILE *f;
				openfile(filename.c_str(), f);
				int w, h;
				fread(&w, 4, 1, f);
				fread(&h, 4, 1, f);
				assert(h >= nSize);
				for (int i = 0; i < nSize; i++) {
					if (pos == TOP) {
						//hp = heightmap + (i + HBORDERSIZE + CHUNKSIZE) * width +
							//HBORDERSIZE + (x - sX) * w;
						hp = heightmap + (i + rp) * width +	HBORDERSIZE + (x - sX) * w;
						seekfile(f, offset + i * w * sizeof(short), SEEK_SET);
						fread(hp, w * sizeof(short), 1, f);
					} 
					else {
						hp = heightmap + i * width + HBORDERSIZE + (x - sX) * w;
						seekfile(f, offset + (h - nSize + i) * w * sizeof(short), SEEK_SET);
						fread(hp, w * sizeof(short), 1, f);
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
				filename = "data/" + getChunkName(x, y) + "/" + TERRAINFILE;
				filename += '0' + level;
				FILE *f;
				openfile(filename.c_str(), f);
				int w, h;
				fread(&w, 4, 1, f);
				fread(&h, 4, 1, f);
				assert(w >= nSize);
				for (int i = 0; i < h; i++) {
					if (pos == LEFT) {
						//hp = heightmap + ((y - sY) * h + i + 1) * width - HBORDERSIZE;
						hp = heightmap + ((y - sY) * h + i + HBORDERSIZE) * width + rp;
						seekfile(f, offset + i * w * sizeof(short), SEEK_SET);
						fread(hp, nSize * sizeof(short), 1, f);
					}
					else {
						hp = heightmap + ((y - sY) * h + i + HBORDERSIZE) * width;
						seekfile(f, offset + ((i + 1) * w - nSize)* sizeof(short), SEEK_SET);
						fread(hp, nSize * sizeof(short), 1, f);
					}
				}
				fclose(f);
			}
		}
	}
}

