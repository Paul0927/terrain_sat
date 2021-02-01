#include "satmanager.h"

SatManager::SatManager(int maxSize) {
	this->maxSize = maxSize;
}


SatManager::~SatManager() {
}

void SatManager::init(int level, int virtualWidth) {
	this->level = level;
	this->virtualWidth = virtualWidth;

	BasePage *last = nullptr;
	for (int i = 0; i < maxSize; i++) {
			BasePage* t = new SatPage();
			if (!i)
				head = t;
			if (i == maxSize - 1)
				tail = t;
			if (last)
				last->next = t;
			t->prev = last;
			last = t;
		}

	for (int i = 0; i < level; i++) {
		vector<BasePage*> tmp;
		tmp.clear();
		for (int j = 0; j < virtualWidth; j++)
			for (int k = 0; k < virtualWidth; k++) {
				tmp.push_back(nullptr);
			}
		pagePos.push_back(tmp);
		virtualWidth >>= 1;
		virtualWidth = (virtualWidth == 0) ? 1 : virtualWidth;
	}
}
