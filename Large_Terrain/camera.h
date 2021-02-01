#pragma once
#include "global.h"
#include <vector>
#include <glm/glm.hpp>

class Camera {
private:
	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 up;
	glm::vec3 horizontal;
	glm::vec3 speed;
	float pitch;
	float yaw;
	float rotateCoe;
	bool modified;
	float grid;
public:
	Camera();
	~Camera();
	void setPos(float x, float y, float z);
	glm::vec3 getPos() const;
	glm::vec3 getUp() const;
	glm::vec3 getHorizontal() const;
	glm::vec3 getDir() const;
	void move(bool keys[], float deltaTime, int width, int height);
	void rotate(float xOff, float yOff);
	float getPitch() const;
	float getYaw() const;
	void revise();
	void setGrid(int grid);
};
