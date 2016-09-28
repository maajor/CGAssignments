#pragma once
#include "../include/model.h"
#include "../include/shader.h"

class MagicCube
{
public:
	MagicCube(unsigned _rank, string unitPath);
	~MagicCube();

	enum Axis { X_Axis, Y_Axis, Z_Axis };
	enum Rotate_Direction { Clockwise, CounterClockwise };

	unsigned rank;//2-6
	void rotateX(unsigned row, float angleInDegree);
	void rotateY(unsigned row, float angleInDegree);
	void rotateZ(unsigned row, float angleInDegree);
	void resetCube(unsigned axis, unsigned row, int direction);
	void render(Shader shader);

private:
	std::vector<std::vector<std::vector<Model>>> _CubeModels;
};

