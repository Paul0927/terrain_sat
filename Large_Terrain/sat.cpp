#include "sat.h"
#include "utils.h"

double SAT::restore(double x, int ep, int htop, int hoff, int tot, bool isMin) {
	double ret;
	if (isMin) {
		ret = pow(x / tot, 1.0 / ep);
		ret = 1 - ret;
	}
	else {
		ret = pow(x, 1.0 / ep);
	}
	ret = ret * htop - hoff;
	return ret;
}

double SAT::getVal(double * data, int x, int y) {
	if (x >= 0 && y >= 0)
		return data[y * w + x];
	else
		return 0;
}

double SAT::getArea(double * data, int lx, int ly, int rx, int ry, int &num) {
	num = (rx - lx + 1) * (ry - ly + 1);
	return getVal(data, rx, ry)
		- getVal(data, rx, ly - 1)
		- getVal(data, lx - 1, ry)
		+ getVal(data, lx - 1, ly - 1);
}

double SAT::getMaxOrMin(int lx, int ly, int rx, int ry, bool isMin) {
	int blx, bly, mlx, mly, brx, bry, mrx, mry;

	blx = lx / CHUNKSIZE;
	bly = ly / CHUNKSIZE;
	mlx = lx - blx * CHUNKSIZE;
	mly = ly - bly * CHUNKSIZE;

	brx = rx / CHUNKSIZE;
	bry = ry / CHUNKSIZE;
	mrx = rx - brx * CHUNKSIZE;
	mry = ry - bry * CHUNKSIZE;

	int x, y;
	double ret = -1000000;
	if (isMin)
		ret = -ret;
	for (x = blx + 1; x < brx; x++)
		for (y = bly + 1; y < bry; y++) {
			if (isMin)
				ret = Min(ret, minHeight[x][y]);
			else
				ret = Max(ret, maxHeight[x][y]);
		}

	int h0;
	if (ly <= bry * h)
		h0 = 0;
	else
		h0 = mly;

	int h1;
	if (ry > (bly + 1) * h)
		h1 = h - 1;
	else
		h1 = mry;

	int w0;
	if (lx <= brx * w)
		w0 = 0;
	else
		w0 = mlx;

	int w1;
	if (rx > (blx + 1) * w)
		w1 = w - 1;
	else
		w1 = mrx;

	//y == bly
	y = bly;
	double tmp;
	int num;
	double* data;
	for (x = blx; x <= brx; x++) {
		if (isMin)
			data = minSAT[x][y];
		else
			data = maxSAT[x][y];

		if (data == nullptr)
			data = nullptr;

		if (x == blx) {	
			tmp = getArea(data, mlx, mly, w1, h1, num);
		}
		else if (x == brx) {
			tmp = getArea(data, w0, mly, mrx, h1, num);
		}
		else {
			tmp = getArea(data, 0, mly, w - 1, h1, num);
		}
		if (isMin)
			ret = Min(ret, restore(tmp, ep, htop, hoff, num, true));
		else 
			ret = Max(ret, restore(tmp, ep, htop, hoff, num, false));
	}
	//y == bry
	y = bry;
	for (x = blx; x <= brx; x++) {
		if (isMin)
			data = minSAT[x][y];
		else
			data = maxSAT[x][y];
		if (data == nullptr)
			data = nullptr;
		if (x == blx) {
			tmp = getArea(data, mlx, h0, w1, mry, num);
		}
		else if (x == brx) {
			tmp = getArea(data, w0, h0, mrx, mry, num);
		}
		else {
			tmp = getArea(data, 0, h0, w - 1, mry, num);
		}
		if (isMin)
			ret = Min(ret, restore(tmp, ep, htop, hoff, num, true));
		else 
			ret = Max(ret, restore(tmp, ep, htop, hoff, num, false));
	}

	//x == blx
	x = blx;
	for (y = bly + 1; y < bry; y++) {
		if (isMin)
			data = minSAT[x][y];
		else
			data = maxSAT[x][y];
		if (data == nullptr)
			data = nullptr;
		tmp = getArea(data, mlx, 0, w1, h - 1, num);
		if (isMin)
			ret = Min(ret, restore(tmp, ep, htop, hoff, num, true));
		else 
			ret = Max(ret, restore(tmp, ep, htop, hoff, num, false));
	}

	//x == brx
	x = brx;
	for (y = bly + 1; y < bry; y++) {
		if (isMin)
			data = minSAT[x][y];
		else
			data = maxSAT[x][y];
		if (data == nullptr)
			data = nullptr;
		tmp = getArea(data, w0, 0, mrx, h - 1, num);
		if (isMin)
			ret = Min(ret, restore(tmp, ep, htop, hoff, num, true));
		else 
			ret = Max(ret, restore(tmp, ep, htop, hoff, num, false));
	}
	return ret;
}

double SAT::getMax(int lx, int ly, int rx, int ry) {
	return getMaxOrMin(lx, ly, rx, ry, false);
}

double SAT::getMin(int lx, int ly, int rx, int ry) {
	return getMaxOrMin(lx, ly, rx, ry, true);
}

SAT::SAT() {
	memset(minSAT, 0, sizeof(minSAT));
	memset(maxSAT, 0, sizeof(maxSAT));
	w = h = CHUNKSIZE;
	htop = hoff = ep = -1;
}

SAT::~SAT() {
	for (int i = 0; i < CHUNKNUMBER; i++)
		for (int j = 0; j < CHUNKNUMBER; j++) {
			if (minSAT[i][j] != nullptr)
				delete minSAT[i][j];
			if (maxSAT[i][j] != nullptr)
				delete maxSAT[i][j];
		}
}

void SAT::loadSATData(int x, int y) {
	if (maxSAT[x][y])
		return;

	string filename = "data/" + getChunkName(x, y) + "/maxDiffSAT";
	FILE *f;
	openfile(filename.c_str(), f);
	if (htop == -1) {
		seekfile(f, 2 * sizeof(int), SEEK_SET);
	 	fread(&htop, sizeof(int), 1, f);
	 	fread(&hoff, sizeof(int), 1, f);
	 	fread(&ep, sizeof(int), 1, f);
	}
	int offset = 5 * sizeof(int);
	maxSAT[x][y] = new double[w * h];
	seekfile(f, offset, SEEK_SET);
	fread(maxSAT[x][y], w * h * sizeof(double), 1, f);
	fclose(f);
	// string filename = "data/" + getChunkName(x, y) + "/minDEMSAT";
	// FILE *f;
	// openfile(filename.c_str(), f);
	// if (htop == -1) {
	// 	seekfile(f, 2 * sizeof(int), SEEK_SET);
	// 	fread(&htop, sizeof(int), 1, f);
	// 	fread(&hoff, sizeof(int), 1, f);
	// 	fread(&ep, sizeof(int), 1, f);
	// }
	// int offset = 5 * sizeof(float);

	// minSAT[x][y] = new double[w * h];
	// maxSAT[x][y] = new double[w * h];

	// seekfile(f, offset, SEEK_SET);
	// fread(minSAT[x][y], w * h * sizeof(double), 1, f);
	// fclose(f);
	
	// filename = "data/" + getChunkName(x, y) + "/maxDEMSAT";
	// openfile(filename.c_str(), f);
	// seekfile(f, offset, SEEK_SET);
	// fread(maxSAT[x][y], w * h * sizeof(double), 1, f);
	// fclose(f);
}

void SAT::loadMaxAndMin() {
	FILE *fp = fopen("maxDiff.txt", "r");
	for (int i = 0; i < CHUNKNUMBER; i++)
		for (int j = 0; j < CHUNKNUMBER; j++)
			fscanf(fp, "%lf", &maxHeight[i][j]);
	fclose(fp);
	// FILE *fp = fopen("maxDEMHeight.txt", "r");
	// for (int i = 0; i < CHUNKNUMBER; i++)
	// 	for (int j = 0; j < CHUNKNUMBER; j++)
	// 		fscanf(fp, "%lf", &maxHeight[i][j]);
	// fclose(fp);
	// fp = fopen("minDEMHeight.txt", "r");
	// for (int i = 0; i < CHUNKNUMBER; i++)
	// 	for (int j = 0; j < CHUNKNUMBER; j++)
	// 		fscanf(fp, "%lf", &minHeight[i][j]);
	// fclose(fp);
}

double SAT::getMaxDiff(int lx, int ly, int rx, int ry) {
	return getMax(lx, ly, rx, ry) - getMin(lx, ly, rx, ry);
}

void SAT::loadSATData(float currentX, float currentZ) {
	float lx, rx, ly, ry;
	float r = VIEWCHUNKNUMBER;
	r /= CHUNKNUMBER;
	lx = currentX - r / 2;
	rx = currentX + r / 2;
	ly = currentZ - r / 2;
	ry = currentZ + r / 2;

	int plx, ply, prx, pry;
	plx = floor(lx * CHUNKNUMBER + 1e-6);
	prx = floor(rx * CHUNKNUMBER + 1e-6);
	ply = floor(ly * CHUNKNUMBER + 1e-6);
	pry = floor(ry * CHUNKNUMBER + 1e-6);
	clamp(plx, 0, CHUNKNUMBER - 1);
	clamp(prx, 0, CHUNKNUMBER - 1);
	clamp(ply, 0, CHUNKNUMBER - 1);
	clamp(pry, 0, CHUNKNUMBER - 1);

	for (int x = plx; x <= prx; x++)
		for (int y = ply; y <= pry; y++)
			loadSATData(x, y);
}
