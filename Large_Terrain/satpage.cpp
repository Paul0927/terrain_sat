#include "satpage.h"
#include "utils.h"


SatPage::SatPage() {
	width = height = CHUNKSIZE;
	data = new double[width * height];
}

SatPage::~SatPage() {
	if (data)
		delete data;
}

void SatPage::loadData(int level, int tx, int ty) {
	int sX, eX, sY, eY;
	sX = tx << level;
	eX = (tx << level) + (1 << level);
	clamp(eX, 0, CHUNKNUMBER);
	sY = ty << level;
	eY = (ty << level) + (1 << level);
	clamp(eY, 0, CHUNKNUMBER);

	htop = -1;
	for (int y = sY; y < eY; y++)
		for (int x = sX; x < eX; x++) {
			string filename = "data/" + getChunkName(x, y) + "/maxDiffSAT";
			filename += '0' + level;
			FILE *f;
			openfile(filename.c_str(), f);
			int w, h;
			fread(&w, 4, 1, f);
			fread(&h, 4, 1, f);
			if (htop == -1) {
				seekfile(f, 2 * sizeof(int), SEEK_SET);
				fread(&htop, sizeof(int), 1, f);
				fread(&hoff, sizeof(int), 1, f);
				fread(&ep, sizeof(int), 1, f);
			}
			seekfile(f, 5 * sizeof(int), SEEK_SET);
			double *p;
			for (int i = 0; i < h; i++) {
				p = (double*)data + ((y - sY) * h + i) * CHUNKSIZE + (x - sX) * w;
				fread(p, w * sizeof(double), 1, f);
			}
			fclose(f);
		}
}

double SatPage::getSatVal(int x, int y) {
	if (x >= 0 && y >= 0)
		return ((double*)data)[y * CHUNKSIZE + x];
	else
		return 0;
}


double SatPage::getAreaMax(int lx, int ly, int rx, int ry) {
	double ret;
	ret = getSatVal(rx, ry) - getSatVal(rx, ly - 1)
		- getSatVal(lx - 1, ry) + getSatVal(lx - 1, ly - 1);
	if (ret < 0)
		ret = 0;
	ret = pow(ret, 1.0 / ep);
	ret = ret * htop - hoff;
	return ret;
}
