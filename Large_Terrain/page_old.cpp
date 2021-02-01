#include "page.h"

Page::Page() : size(0), level(0), used(false) {
	pixels = nullptr;
}

Page::~Page() {
	if (pixels)
		delete pixels;
}

void Page::setUsed() {
	used = true;
}

bool Page::getUsed() {
	return used;
}

void Page::resetUsed() {
	used = false;
}

void Page::setPos(int x, int y) {
	this->x = x;
	this->y = y;
}

int Page::getX() const {
	return x;
}

int Page::getY() const {
	return y;
}

void Page::setOpos(int ox, int oy) {
	this->ox = ox;
	this->oy = oy;
}

int Page::getOx() const {
	return ox;
}

int Page::getOy() const {
	return oy;
}

unsigned char * Page::getPixels() {
	return pixels;
}

int Page::getChannels() const{
	return channels;
}

void Page::setLevel(int level) {
	this->level = level;
}

int Page::getLevel() const {
	return level;
}
