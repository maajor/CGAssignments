#include "MagicCube.h"


MagicCube::MagicCube(unsigned _rank, string unitPath)
{
	this->rank = _rank;
	this->_CubeModels = std::vector<std::vector<std::vector<Model>>>(_rank, std::vector<std::vector<Model> >(_rank, std::vector<Model>(_rank, Model(unitPath))));
	for (int i = 0; i < _rank; i++){
		for (int j = 0; j < _rank; j++){
			for (int k = 0; k < _rank; k++){
				_CubeModels[i][j][k] = Model(unitPath);
				float xpos = (i - this->rank / 2.0f) + 0.5f;
				float ypos = (j - this->rank / 2.0f) + 0.5f;
				float zpos = (k - this->rank / 2.0f) + 0.5f;
				_CubeModels[i][j][k].modelTransform = glm::translate(_CubeModels[i][j][k].modelTransform, glm::vec3(xpos, ypos, zpos));
			}
		}
	}
}


MagicCube::~MagicCube()
{
}

void MagicCube::rotateX(unsigned row, float angleInDegree){
	if (row >= this->rank)
		return;
	for (int j = 0; j < this->rank; j++){
		for (int k = 0; k < this->rank; k++){
			glm::mat4 rotate;
			rotate = glm::rotate(rotate, angleInDegree, glm::vec3(1.0f, 0.0f, 0.0f));
			_CubeModels[row][j][k].modelTransform = rotate * _CubeModels[row][j][k].modelTransform;
		}
	}
}
void MagicCube::rotateY(unsigned row, float angleInDegree){
	if (row >= this->rank)
		return;
	for (int j = 0; j < this->rank; j++){
		for (int k = 0; k < this->rank; k++){
			glm::mat4 rotate;
			rotate = glm::rotate(rotate, angleInDegree, glm::vec3(0.0f, 1.0f, 0.0f));
			_CubeModels[j][row][k].modelTransform = rotate * _CubeModels[j][row][k].modelTransform;
		}
	}
}
void MagicCube::rotateZ(unsigned row, float angleInDegree){
	if (row >= this->rank)
		return;
	for (int j = 0; j < this->rank; j++){
		for (int k = 0; k < this->rank; k++){
			glm::mat4 rotate;
			rotate = glm::rotate(rotate, angleInDegree, glm::vec3(0.0f, 0.0f, 1.0f));
			_CubeModels[j][k][row].modelTransform = rotate * _CubeModels[j][k][row].modelTransform;
		}
	}
}
void MagicCube::resetCube(unsigned axis, unsigned row, int direction){
}
void MagicCube::render(Shader shader){
	for (int i = 0; i < this->rank; i++){
		for (int j = 0; j < this->rank; j++){
			for (int k = 0; k < this->rank; k++){
				_CubeModels[i][j][k].Draw(shader);
			}
		}
	}
}
