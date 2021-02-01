#include "basepage.h"

BasePage::BasePage() {
}

BasePage::~BasePage() {
}

int BasePage::getLevel() const {
	return level;
}

int BasePage::getVx() const {
	return vx;
}

int BasePage::getVy() const {
	return vy;
}

int BasePage::getPx() const {
	return px;
}

int BasePage::getPy() const {
	return py;
}

void BasePage::setVirtualPos(int level, int x, int y) {
	this->level = level;
	this->vx = x;
	this->vy = y;
}

void BasePage::setPhysicalPos(int x, int y) {
	this->px = x;
	this->py = y;
}

int BasePage::getWidth() const {
	return width;
}

int BasePage::getHeight() const {
	return height;
}

void * BasePage::getData() {
	return data;
}
