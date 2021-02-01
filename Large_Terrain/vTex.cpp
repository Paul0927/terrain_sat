#include "vTex.h"
#include "utils.h"

VTex::VTex() {
}

VTex::~VTex() {
	//for (auto ptex : ptexes)
		//delete ptex;
	delete ptex;
}

void VTex::init(int pageSize, int virtualWidth, string dataname, int maxLevel) {
	this->pageSize = pageSize;
	this->virtualWidth = virtualWidth;

	ptable.clear();
	w.clear();
	h.clear();
	//latest.clear();
	length.clear();
	int tmp;
	for (int i = 0; i < maxLevel; i++) {
		length.push_back((pageSize >> i) * virtualWidth);
		tmp = virtualWidth >> i;
		tmp = (tmp <= 0) ? 1 : tmp;
		w.push_back(tmp);
		h.push_back(tmp);
		ptable.push_back(new float[w[i] * h[i] * 4]);
		memset(ptable.back(), 0, w[i] * h[i] * 4 * 4);
		//latest.push_back(new bool[w[i] * h[i]]);
	}
	
	this->dataname = dataname;
	this->maxLevel = maxLevel;

	if (dataname[0] == 'd') {
		ptex = new PTex(pageSize, VIEWCHUNKNUMBER * 0.5, dataname);
		ptex->init(maxLevel, virtualWidth);
		satManager = new SatManager(VIEWCHUNKNUMBER * VIEWCHUNKNUMBER * 0.25);
		satManager->init(maxLevel, virtualWidth);
		FILE *f, *f1, *f2;
		string filename, filename1, filename2;
		for(int x = 0; x < CHUNKNUMBER; x++)
			for (int y = 0; y < CHUNKNUMBER; y++)
			{
				filename = "data/" + getChunkName(x, y) + "/errList";
				filename1 = "data/" + getChunkName(x, y) + "/minH";
				filename2 = "data/" + getChunkName(x, y) + "/maxH";
				openfile(filename.c_str(), f);
				openfile(filename1.c_str(), f1);
				openfile(filename2.c_str(), f2);
				for (int i = 0; i < LEVELOFTERRAIN; i++) {
					fread(&eList[x][y][i], sizeof(float), 1, f);
					eList[x][y][i] /= 10;
					fread(&minH[x][y][i], sizeof(float), 1, f1);
					minH[x][y][i] /= 10;
					fread(&maxH[x][y][i], sizeof(float), 1, f2);
					maxH[x][y][i] /= 10;
				}
			
				fclose(f);
				fclose(f1);
				fclose(f2);
			}

	}
	else {
		ptex = new PTex(pageSize, VIEWCHUNKNUMBER * 0.40, dataname);
		ptex->init(maxLevel, virtualWidth);
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel - 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, w[0], h[0], 0,
		GL_RGBA, GL_FLOAT, nullptr);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	loadingPtex.clear();
	loadingSat.clear();
	ptexHandles.clear();
	satHandles.clear();
}

HANDLE VTex::event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
CRITICAL_SECTION VTex::criticalSection;

void updateList(int k, int x, int y, int wk, set<pair<int, int> >&loadingSet, vector<ThreadInfo>&handleList, BaseList* baselist, VTex* vtex) {
	if (loadingSet.find(make_pair(k, y * wk + x)) != loadingSet.end())
		return;
	BasePage *tp;
	tp = baselist->findPage(k, x, y);
	if (tp != nullptr) {
		baselist->moveFront(tp);
	}
	else {
		//if (vtex)
		//	printf("ptex ");
		//else
		//	printf("sat ");
		//printf("%d %d %d\n", k, x, y);
		//if (!vtex && k == 0 && x == 1 && y == 1)
		//	x = x;
		tp = baselist->getReplacePage();
		if (vtex)
			vtex->unmap(tp);

		UpdateInfo uInfo;
		ThreadInfo tInfo;
		uInfo.level = k;
		uInfo.x = x;
		uInfo.y = y;
		uInfo.page = tInfo.page = tp;
		loadingSet.insert(make_pair(k, y * wk + x));
		tInfo.handle = (HANDLE)_beginthreadex(nullptr, 0, VTex::updatePage, &uInfo, 0, nullptr);
		assert(tInfo.handle != 0);
		handleList.push_back(tInfo);
		WaitForSingleObject(VTex::event, INFINITE);
	}
}

void VTex::update(int k, int x, int y, bool needUpdateSat) {
	updateList(k, x, y, w[k], loadingPtex, ptexHandles, ptex, this);
	if (dataname[0] == 'd' && needUpdateSat)
		updateList(k, x, y, w[k], loadingSat, satHandles, satManager, nullptr);
}

void VTex::update(float currentX, float currentY, float currentH, float hLevel[], unsigned char hLevel1[],
				  glm::mat4 matrix, int geoLevel[][1<<LEVEL-1], bool geometryclipmap, bool needUpdate[][CHUNKNUMBER]) {
    
	float lx, rx, ly, ry;
	float r = VIEWCHUNKNUMBER;
	r /= CHUNKNUMBER;
	lx = currentX - r / 2;
	rx = currentX + r / 2;
	ly = currentY - r / 2;
	ry = currentY + r / 2;

	int plx, ply, prx, pry;
	plx = floor(lx * w[0] + 1e-6);
	prx = floor(rx * w[0] + 1e-6);
	ply = floor(ly * h[0] + 1e-6);
	pry = floor(ry * h[0] + 1e-6);
	clamp(plx, 0, w[0] - 1);
	clamp(prx, 0, w[0] - 1);
	clamp(ply, 0, h[0] - 1);
	clamp(pry, 0, h[0] - 1);

	string tFile;
	
	float lastErr;
	const float errThreshold = 2;
	int l, last;
	bool ok[LEVELOFTERRAIN];
	for (int x = plx; x <= prx; x++)
		for (int y = ply; y <= pry; y++) {
			if (!needUpdate[x][y])
				continue;
			
				int cx = x * CHUNKREALSIZE + CHUNKREALSIZE / 2;
				int cy = y * CHUNKREALSIZE + CHUNKREALSIZE / 2;
				int vx = currentX * CHUNKNUMBER * CHUNKREALSIZE;
				int vy = currentY * CHUNKNUMBER * CHUNKREALSIZE;

				lastErr = 0;
				glm::vec4 p0, p1, p2;
				p0.x = cx - vx;
				p0.z = cy - vy;
				float Dp = sqrt(p0.x * p0.x + p0.z * p0.z);
				float s = CHUNKREALSIZE;
				float d = 1 - 1.41421356 * s / 2 / Dp;
				for (int i = 1; i < LEVELOFTERRAIN; i++) {
					if (lastErr <= eList[x][y][i]) {
						lastErr = eList[x][y][i];
						ok[i] = true;
					}
					else {
						ok[i] = false;
						lastErr = eList[x][y][i];
						ok[i] = true;
					}
				}
				ok[0] = true;

				if (d <= 1e-6) {
					hLevel[y * CHUNKNUMBER + x] = hLevel1[y * CHUNKNUMBER + x] = l = 0;
				}
				else
				{
					float h;
					if (minH[x][y][0] <= currentH && maxH[x][y][0] >= currentH)
						h = currentH;
					else if (minH[x][y][0] > currentH)
						h = minH[x][y][0];
					else
						h = maxH[x][y][0];

					p0.x = cx - vx;
					p0.z = cy - vy;
					p0.y = h - currentH;
					p0 = p0 * d;
					p0.y += currentH;
					p0.x += vx;
					p0.z += vy;
					p0.w = 1;
					p1 = p0;
					p0 = matrix * p0;
					p2 = p0;
					p0 /= p0.w;

					last = -1;
					
				for (int i = LEVELOFTERRAIN - 1; i >= 0; i--) {
					if (!ok[i])
						continue;
					l = i;
					float e = eList[x][y][i];
					p1 = p2;
					p1.x += matrix[1][0] * e;
					p1.y += matrix[1][1] * e;
					p1.z += matrix[1][2] * e;
					p1.w += matrix[1][3] * e;
					p1 /= p1.w;
					double xPixel = (fabs(p0.x - p1.x) / 2) * WIDTH;
					double yPixel = (fabs(p0.y - p1.y) / 2) * HEIGHT;
					double pixel = xPixel * xPixel + yPixel * yPixel;
					pixel = sqrt(pixel);
					if (pixel <= errThreshold) {
						hLevel[y * CHUNKNUMBER + x] = hLevel1[y * CHUNKNUMBER + x] = l;
						if (last != -1) {
							
							hLevel[y * CHUNKNUMBER + x] += (errThreshold - pixel) / (lastErr - pixel);
						}
						break;
					}
					else {
						last = i;
						lastErr = pixel;
					}
				}
				if (hLevel[y * CHUNKNUMBER + x] == -1) {
					hLevel[y * CHUNKNUMBER + x] = hLevel1[y * CHUNKNUMBER + x] = 0;
					last = -1;
			 	}
			}

			l = floor(hLevel[y * CHUNKNUMBER + x]);
			update(l, x >> l, y >> l, true);
			if (hLevel1[y * CHUNKNUMBER + x] != l) {
				l = hLevel1[y * CHUNKNUMBER + x];
				update(l, x >> l, y >> l, false);
			}
		}
}

GLuint VTex::generateTex() {
	glBindTexture(GL_TEXTURE_2D, tex);
	
	for (int i = 0; i < maxLevel; i++)
		glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA32F_ARB, w[i], h[i], 0, 
			GL_RGBA, GL_FLOAT, ptable[i]);

	glBindTexture(GL_TEXTURE_2D, 0);
	return tex;
}

GLuint VTex::getTex() {
	return tex;
}

void VTex::clear() {
	ptex->clearCount();
	if (dataname[0] == 'd') {
		satManager->clearCount();
	}
}

PTex* VTex::getPtex() {
	return ptex;
}

int VTex::getMaxLevel() const {
	return maxLevel;
}

void VTex::map(BasePage* tp) {
	int level, x, y;
	level = tp->getLevel();
	x = tp->getVx();
	y = tp->getVy();
	float *pt;
	int tx, ty;
	tx = x;
	ty = y;
	//printf("map %d %d %d:\n", level, x, y);
	int tot = 0;
	for (int i = level; i >= 0; i--) {
		int lx, rx, ly, ry;
		int n = CHUNKNUMBER >> i;
		n = (n == 0) ? 1 : n;
		lx = tx;
		ly = ty;
		rx = lx + (1 << level - i);
		clamp(rx, 0, n);
		ry = ly + (1 << level - i);
		clamp(ry, 0, n);
		//printf("level %d range: %d %d %d %d\n\n", i, lx, rx, ly, ry);
		for (int k1 = lx; k1 < rx; k1++)
			for (int k2 = ly; k2 < ry; k2++) {
				pt = ptable[i] + (k2 * w[i] + k1) * 4;
				if (pt[0] >= (float)length[level] / ptex->getPixelSize())
					continue;
				tot++;
				if (i == 0 && k1 == 2 && k2 == 4)
					k1 = k1;
				pt = ptable[i] + (k2 * w[i] + k1) * 4;
				pt[0] = (float)length[level] / ptex->getPixelSize();
				pt[1] = 0;
				pt[2] = (float)tp->getPx() / ptex->getPhysicalWidth() -
						(float)lx / w[i] * pt[0];
				pt[2] += ((float)ptex->getBorderSize())
						/ ptex->getPixelSize();
				pt[3] = (float)tp->getPy() / ptex->getPhysicalWidth() -
						(float)ly / h[i] * pt[0];
				pt[3] += ((float)ptex->getBorderSize())
						/ ptex->getPixelSize();
			}
		ty <<= 1;
		tx <<= 1;
		if (!tot)
			break;
		else
			tot = 0;
	}
}

void VTex::unmap(BasePage* tp) {
	int level, x, y;
	level = tp->getLevel();
	if (level == -1)
		return;
	x = tp->getVx();
	y = tp->getVy();
	if (level == 0 && x == 2 && y == 4)
		x = x;
	float *pt;
	int tx, ty;
	tx = x;
	ty = y;
	int tot = 0;
	for (int i = level; i >= 0; i--) {
		int lx, rx, ly, ry;
		int n = CHUNKNUMBER >> i;
		n = (n == 0) ? 1 : n;
		lx = tx;
		ly = ty;
		rx = lx + (1 << level - i);
		clamp(rx, 0, n);
		ry = ly + (1 << level - i);
		clamp(ry, 0, n);
		for (int k1 = lx; k1 < rx; k1++)
			for (int k2 = ly; k2 < ry; k2++) {	
				pt = ptable[i] + (k2 * w[i] + k1) * 4;
				if (pt[0] >= (float)length[level] / ptex->getPixelSize())
					continue;
				tot++;
				pt = ptable[i] + (k2 * w[i] + k1) * 4;
				pt[0] = 0;
			}
		tx <<= 1;
		ty <<= 1;
		if (!tot)
			break;
		else
			tot = 0;
	}
	for (int i = level + 1; i < maxLevel; i++) {
		x >>= 1;
		y >>= 1;
		if ((tp = ptex->findPage(i, x, y)) != nullptr) {
			map(tp);
			break;
		}
	}
}

void VTex::loadCoarsest() {
	int level = maxLevel - 1;
	int n = virtualWidth >> level;
	n = (n == 0) ? 1 : n;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++) {
			BasePage *tp = ptex->getReplacePage();
			tp->setVirtualPos(level, i, j);
			tp->loadData(level, i, j);
			ptex->update(level, i, j, tp);
			ptex->insert(level, i, j, tp);
			map(tp);
			if (dataname[0] == 'd') {
				tp = satManager->getReplacePage();
				tp->setVirtualPos(level, i, j);
				tp->loadData(level, i, j);
				satManager->insert(level, i, j, tp);
			}
		}
}

void checkState(vector<int>&w, set<pair<int, int> > &loadingSet, vector<ThreadInfo> &handleList, BaseList* baselist, VTex* vtex) {
	DWORD exitCode;
	for (int i = 0; i < handleList.size(); i++) {
		GetExitCodeThread(handleList[i].handle, &exitCode);
		if (exitCode != STILL_ACTIVE) {
			//printf("%d\n", exitCode);
			BasePage* page = handleList[i].page;
			int level, x, y;
			level = page->getLevel();
			x = page->getVx();
			y = page->getVy();
			if (vtex)
				((PTex*)baselist)->update(level, x, y, page);
			baselist->insert(level, x, y, page);
			if (vtex)
				vtex->map(page);
			loadingSet.erase(make_pair(level, y * w[level] + x));
			CloseHandle(handleList[i].handle);
			handleList.erase(handleList.begin() + i);
			i--;
		}
	}
}

void VTex::checkThreadState() {
	checkState(w, loadingPtex, ptexHandles, ptex, this);
	if (dataname[0] == 'd')
		checkState(w, loadingSat, satHandles, satManager, nullptr);
}

unsigned int VTex::updatePage(void* pm) {
	//printf("Thread %d begins...\n", GetCurrentThreadId());
	UpdateInfo *uInfo = (UpdateInfo*)pm;
	int level = uInfo->level;
	int x = uInfo->x;
	int y = uInfo->y;
	BasePage* page = uInfo->page;
	SetEvent(event);
	//printf("ThreadID:%d Blocks: %d %d %d %d\n", GetCurrentThreadId(), level, x, y, page->getWidth());
	page->setVirtualPos(level, x, y);
	page->loadData(level, x, y);
	//printf("Thread %d ends...\n", GetCurrentThreadId());
	return 1;
}

void VTex::free() {
	CloseHandle(event);
	DeleteCriticalSection(&criticalSection);
}

void VTex::initThread() {
	InitializeCriticalSection(&criticalSection);
}

tuple<double, int, float, float> VTex::getSatMax(int lx, int ly, int rx, int ry, float hLevel[]) {
	double ret = -1000000;

	int blx, bly, mlx, mly, brx, bry, mrx, mry;

	blx = lx / CHUNKSIZE;
	bly = ly / CHUNKSIZE;
	mlx = lx - blx * CHUNKSIZE;
	mly = ly - bly * CHUNKSIZE;

	brx = rx / CHUNKSIZE;
	bry = ry / CHUNKSIZE;
	mrx = rx - brx * CHUNKSIZE;
	mry = ry - bry * CHUNKSIZE;
	int minlevel = LEVELOFTERRAIN;
	float minHval = 40000;
	float maxHval = 0.0;
	for (int x = blx; x <= brx; x++)
		for (int y = bly; y <= bry; y++) {
			int level = (int)hLevel[y * CHUNKNUMBER + x];
			minlevel = min(minlevel, level);
			maxHval = max(maxHval, maxH[x][y][level]);
			minHval = min(minHval, minH[x][y][level]);
			BasePage *p = nullptr;
			int tx = x >> level, ty = y >> level, l = level;
			while (!p) {
				p = satManager->findPage(l, tx, ty);
				if (!p) {
					l++;
					tx >>= 1;
					ty >>= 1;
				}
			}
			SatPage *tp = (SatPage*)p;
			int localLx, localLy, localRx, localRy;
			localLx = (x == blx) ? lx : x * CHUNKSIZE;
			localRx = (x == brx) ? rx : (x + 1) * CHUNKSIZE - 1;
			localLy = (y == bly) ? ly : y * CHUNKSIZE;
			localRy = (y == bry) ? ry : (y + 1) * CHUNKSIZE - 1;

			localLx = localLx - (tx << l) * CHUNKSIZE >> l;
			localRx = localRx - (tx << l) * CHUNKSIZE >> l;
			localLy = localLy - (ty << l) * CHUNKSIZE >> l;
			localRy = localRy - (ty << l) * CHUNKSIZE >> l;
			ret = Max(ret, tp->getAreaMax(localLx, localLy, localRx, localRy) / (1 << l));
		}
	return {ret, minlevel, minHval, maxHval };
}