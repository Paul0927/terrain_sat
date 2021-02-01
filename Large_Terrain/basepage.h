#pragma once
class BasePage {
protected:
	int level, vx, vy;
	int px, py;

	int width, height;
	void *data;

public:
	BasePage *next, *prev;

	BasePage();
	~BasePage();	
	int getLevel() const;
	int getVx() const;
	int getVy() const;
	int getPx() const;
	int getPy() const;
	void setVirtualPos(int level, int x, int y);
	void setPhysicalPos(int x, int y);
	int getWidth() const;
	int getHeight() const;
	void *getData();
	virtual void loadData(int level, int x, int y) {};
};

