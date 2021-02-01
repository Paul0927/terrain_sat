//Page Table
#pragma once

#include "global.h"
#include "page.h"
#include "pTex.h"
#include "sat.h"
#include "treeNode.h"
#include "satmanager.h"


class ThreadInfo {
public:
	BasePage* page;
	HANDLE handle;
	BasePage* satPage;
};


class VTex {
private:
	vector<float*> ptable;
	//vector<bool*> latest;
	vector<int> w, h;
	vector<int> length;
	GLuint tex;
	PTex* ptex;
	SatManager* satManager;
	int pageSize, virtualWidth;
	int maxLevel;
	string dataname;
	set<pair<int, int> > loadingPtex;
	set<pair<int, int> > loadingSat;
	vector<ThreadInfo> ptexHandles;
	vector<ThreadInfo> satHandles;

public:
	static HANDLE event;
	static CRITICAL_SECTION criticalSection;
	VTex();
	~VTex();
	void init(int pageSize, int virtualWidth, string dataname, int maxLevel);
	void update(int k, int x, int y, bool needUpdateSat);
	void update(float currentX, float currentY, float currentH, float hLevel[], unsigned char hLevel1[],
				glm::mat4 matrix, int geoLevel[][1<<LEVEL-1], bool geometryclipmap, bool needUpdate[][CHUNKNUMBER]);
	GLuint generateTex();
	GLuint getTex();
	void clear();
	PTex * getPtex();
	//vector<PTex*> getPtexes();
	int getMaxLevel() const;
	void map(BasePage * tp);
	void unmap(BasePage * tp);
	void loadCoarsest();
	void checkThreadState();
	static unsigned int __stdcall updatePage(void * pm);
	static void free();
	static void initThread();
	tuple<double, int, float, float> getSatMax(int lx, int ly, int rx, int ry, float hLevel[]);
	float eList[CHUNKNUMBER][CHUNKNUMBER][LEVELOFTERRAIN];
	float minH[CHUNKNUMBER][CHUNKNUMBER][LEVELOFTERRAIN];
	float maxH[CHUNKNUMBER][CHUNKNUMBER][LEVELOFTERRAIN];
};

class UpdateInfo {
public:
	int level, x, y;
	BasePage* page;
};