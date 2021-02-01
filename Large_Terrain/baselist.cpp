#include "baselist.h"


BaseList::BaseList() {
	head = tail = nullptr;
}


BaseList::~BaseList() {
}

void BaseList::moveFront(BasePage * p) {
	if (p == head)
		return;
	if (p->prev)
		p->prev->next = p->next;
	if (p->next)
		p->next->prev = p->prev;

	if (p == tail)
		tail = tail->prev;
	
	head->prev = p;
	p->next = head;
	head = p;
	head->prev = nullptr;
}

void BaseList::insert(int level, int x, int y, BasePage* p) {
	int n = virtualWidth >> level;
	n = (n == 0) ? 1 : n;
	pagePos[level][y * n + x] = p;

	head->prev = p;
	p->next = head;
	head = p;
	head->prev = nullptr;
}

BasePage* BaseList::getReplacePage() {
	if (tail == first) {
		printf("physical pages are not enough.");
		//exit(0);
	}
	BasePage* p = tail;
	while (p->getLevel() == this->level - 1) {	
		moveFront(p);
		p = tail;	
		if (tail == first) {
			printf("physical pages are not enough.");
			//exit(0);
		}
	}
	tail = tail->prev;
	tail->next = nullptr;
	int level = p->getLevel();
	if (level == -1)
		return p;
	int x = p->getVx();
	int y = p->getVy();
	int n = virtualWidth >> level;
	n = (n == 0) ? 1 : n;
	pagePos[level][y * n + x] = nullptr;
	return p;
}

BasePage* BaseList::findPage(int level, int x, int y) {
	int n = virtualWidth >> level;
	n = (n == 0) ? 1 : n;
	return pagePos[level][y * n + x];
}

void BaseList::clearCount() {
	first = head;
}