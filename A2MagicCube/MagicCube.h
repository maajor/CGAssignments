#pragma once
#include "../include/model.h"

class MagicCube
{
public:
	MagicCube();
	~MagicCube();

	enum Axis { X_Axis, Y_Axis, Z_Axis };
	enum Rotate_Direction { Clockwise, CounterClockwise };

	unsigned rank;//2-6
	void rotateX(unsigned row, float angleInDegree);
	void rotateY(unsigned row, float angleInDegree);
	void rotateZ(unsigned row, float angleInDegree);
	void resetCube(unsigned axis, unsigned row, int direction);

private:
	
};

