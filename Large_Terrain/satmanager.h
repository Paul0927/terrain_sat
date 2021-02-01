#pragma once
#include "baselist.h"
#include "satpage.h"

class SatManager : public BaseList{
public:
	SatManager(int maxSize);
	~SatManager();
	virtual void init(int level, int virtualWidth);
};

