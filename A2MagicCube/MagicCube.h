#pragma once
#include "../include/model.h"
#include "../include/shader.h"
#include "ray.h"

class MagicCube
{
public:
	MagicCube();
	MagicCube(unsigned _rank, string unitPath);
	~MagicCube();

	enum Axis { X_Axis, Y_Axis, Z_Axis };
	enum Rotate_Direction { Clockwise, CounterClockwise };

	unsigned rank;//2-6
	void rotateXIncrement(unsigned row, float angleInDegree);
	void rotateYIncrement(unsigned row, float angleInDegree);
	void rotateZIncrement(unsigned row, float angleInDegree);
	void rotateXTo(unsigned row, float angleInDegree);
	void rotateYTo(unsigned row, float angleInDegree);
	void rotateZTo(unsigned row, float angleInDegree);
	void resetTransform();
	void resetCube(unsigned axis, unsigned row, float angle);
	void render(Shader shader);
	bool findHit(Ray hitray, glm::vec3 &hitIndex, int &side);



private:
	std::vector<std::vector<std::vector<Model>>> _CubeModels;
	std::vector<std::vector<glm::mat4>> _tempRotMatrix;
	void MagicCube::calIndexRotate(int i, int j, int rank, int& outI, int& outJ, float rot);
};

