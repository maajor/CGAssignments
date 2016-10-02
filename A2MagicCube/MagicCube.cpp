#include "MagicCube.h"


MagicCube::MagicCube(unsigned _rank, string unitPath)
{
	this->rank = _rank;
	this->_CubeModels = std::vector<std::vector<std::vector<Model>>>(_rank, std::vector<std::vector<Model> >(_rank, std::vector<Model>(_rank, Model(unitPath))));
	for (int i = 0; i < _rank; i++){
		for (int j = 0; j < _rank; j++){
			for (int k = 0; k < _rank; k++){
				_CubeModels[i][j][k] = Model(unitPath);
				_CubeModels[i][j][k].bbx[0] = glm::vec3(-0.5f, 0, -0.5f);
				_CubeModels[i][j][k].bbx[1] = glm::vec3(0.5f, 1.0f, 0.5f);
				float xpos = (i - this->rank / 2.0f) + 0.5f;
				float ypos = (j - this->rank / 2.0f) + 0.5f;
				float zpos = (k - this->rank / 2.0f) + 0.5f;
				_CubeModels[i][j][k].modelTransform = glm::translate(_CubeModels[i][j][k].modelTransform, glm::vec3(xpos, ypos, zpos));
				_CubeModels[i][j][k].modelTranslate = _CubeModels[i][j][k].modelTransform;
			}
		}
	}
}
MagicCube::MagicCube(){
	this->rank = 3;
	this->_CubeModels = std::vector<std::vector<std::vector<Model>>>(3, std::vector<std::vector<Model> >(3, std::vector<Model>(3, Model())));
}

MagicCube::~MagicCube()
{
}

void MagicCube::rotateXIncrement(unsigned row, float angleInDegree){
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
void MagicCube::rotateYIncrement(unsigned row, float angleInDegree){
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
void MagicCube::rotateZIncrement(unsigned row, float angleInDegree){
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

void MagicCube::rotateXTo(unsigned row, float angleInDegree){
	if (row >= this->rank)
		return;
	for (int j = 0; j < this->rank; j++){
		for (int k = 0; k < this->rank; k++){
			glm::mat4 rotate;
			rotate = glm::rotate(rotate, angleInDegree, glm::vec3(1.0f, 0.0f, 0.0f));
			_CubeModels[row][j][k].modelTransform = rotate * _CubeModels[row][j][k].modelRotation * _CubeModels[row][j][k].modelTranslate;
		}
	}
}
void MagicCube::rotateYTo(unsigned row, float angleInDegree){
	if (row >= this->rank)
		return;
	for (int j = 0; j < this->rank; j++){
		for (int k = 0; k < this->rank; k++){
			glm::mat4 rotate;
			rotate = glm::rotate(rotate, angleInDegree, glm::vec3(0.0f, 1.0f, 0.0f));
			_CubeModels[j][row][k].modelTransform = rotate * _CubeModels[j][row][k].modelRotation* _CubeModels[j][row][k].modelTranslate;
		}
	}
}
void MagicCube::rotateZTo(unsigned row, float angleInDegree){
	if (row >= this->rank)
		return;
	for (int j = 0; j < this->rank; j++){
		for (int k = 0; k < this->rank; k++){
			glm::mat4 rotate;
			rotate = glm::rotate(rotate, angleInDegree, glm::vec3(0.0f, 0.0f, 1.0f));
			_CubeModels[j][k][row].modelTransform = rotate * _CubeModels[j][k][row].modelRotation* _CubeModels[j][k][row].modelTranslate;
		}
	}
}

void MagicCube::resetTransform(){
	for (int i = 0; i < this->rank; i++){
		for (int j = 0; j < this->rank; j++){
			for (int k = 0; k < this->rank; k++){
				_CubeModels[i][j][k].modelTransform = _CubeModels[i][j][k].modelTranslate * _CubeModels[i][j][k].modelRotation;
			}
		}
	}
}
void MagicCube::resetCube(unsigned axis, unsigned row, float angle){
	glm::mat4 rotate;
	float  angleInDegree = (std::floor((angle + 0.785)/ 1.57)) * 1.5707963f;
	if (axis == 0){
		rotate = glm::rotate(rotate, angleInDegree, glm::vec3(1.0f, 0.0f, 0.0f));
		for (int i = 0; i < this->rank; i++){
			for (int j = 0; j < this->rank; j++){
				_CubeModels[row][i][j].modelRotation = _CubeModels[row][i][j].modelRotation * rotate;
				_CubeModels[row][i][j].modelTransform = _CubeModels[row][i][j].modelTranslate * _CubeModels[row][i][j].modelRotation;

			}
		}
	}
	else if (axis == 1){
		rotate = glm::rotate(rotate, angleInDegree, glm::vec3(0.0f, 1.0f, 0.0f));
		for (int i = 0; i < this->rank; i++){
			for (int j = 0; j < this->rank; j++){
				_CubeModels[i][row][j].modelRotation = _CubeModels[i][row][j].modelRotation * rotate;
				_CubeModels[i][row][j].modelTransform = _CubeModels[i][row][j].modelTranslate * _CubeModels[i][row][j].modelRotation;

			}
		}
	}
	else {
		rotate = glm::rotate(rotate, angleInDegree, glm::vec3(0.0f, 0.0f, 1.0f));
		for (int i = 0; i < this->rank; i++){
			for (int j = 0; j < this->rank; j++){
				_CubeModels[i][j][row].modelRotation = _CubeModels[i][j][row].modelRotation * rotate;
				_CubeModels[i][j][row].modelTransform = _CubeModels[i][j][row].modelTranslate * _CubeModels[i][j][row].modelRotation;

			}
		}
	}

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

bool MagicCube::findHit(Ray hitray, glm::vec3 &hitIndex, int &side){
	glm::vec3 hitIndexTemp(-1, -1, -1);
	float minHitDist = FLT_MAX;
	for (int i = 0; i < this->rank; i++){
		for (int j = 0; j < this->rank; j++){
			for (int k = 0; k < this->rank; k++){
				float hitDist = 0;
				glm::vec3 currentBbx[2];
				currentBbx[0] = glm::vec3(_CubeModels[i][j][k].modelTranslate * glm::vec4(_CubeModels[i][j][k].bbx[0], 1));
				currentBbx[1] = glm::vec3(_CubeModels[i][j][k].modelTranslate * glm::vec4(_CubeModels[i][j][k].bbx[1], 1));
				if (hitray.checkHit(hitDist, currentBbx, side))
				{
					if (hitDist < minHitDist)
					{
						std::cout << "this hit dist" << hitDist << " " << i << " " << j << " " << k << std::endl;
						minHitDist = hitDist;
						hitIndexTemp = glm::vec3(i, j, k);
					}
				}
			}
		}
	}

	if (hitIndexTemp.x == -1){
		return false;
	}
	else{
		hitIndex = hitIndexTemp;
		return true;
	}
}
