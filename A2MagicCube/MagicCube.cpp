#include "MagicCube.h"


MagicCube::MagicCube(unsigned _rank, string unitPath)
{
	this->rank = _rank;
	this->_CubeModels = std::vector<std::vector<std::vector<Model>>>(_rank, std::vector<std::vector<Model> >(_rank, std::vector<Model>(_rank, Model(unitPath))));
	this->_tempRotMatrix = std::vector<std::vector<glm::mat4>>(_rank, std::vector<glm::mat4>(_rank, glm::mat4()));
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
			_CubeModels[row][j][k].modelTransform = rotate * _CubeModels[row][j][k].modelTranslate * _CubeModels[row][j][k].modelRotation;
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
			_CubeModels[j][row][k].modelTransform = rotate * _CubeModels[j][row][k].modelTranslate * _CubeModels[j][row][k].modelRotation;
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
			_CubeModels[j][k][row].modelTransform = rotate *  _CubeModels[j][k][row].modelTranslate * _CubeModels[j][k][row].modelRotation;
			//_CubeModels[j][k][row].modelTransform = _CubeModels[j][k][row].modelTranslate * rotate * _CubeModels[j][k][row].modelRotation;
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
				int in, jn;
				calIndexRotate(i, j, this->rank, in, jn, angleInDegree);
				_CubeModels[row][i][j].modelTransform = _CubeModels[row][i][j].modelTranslate * rotate * _CubeModels[row][in][jn].modelRotation;
				_tempRotMatrix[i][j] = _CubeModels[row][i][j].modelRotation;

			}
		}
		for (int i = 0; i < this->rank; i++){
			for (int j = 0; j < this->rank; j++){
				int in, jn;
				calIndexRotate(i, j, this->rank, in, jn, angleInDegree);
				_CubeModels[row][i][j].modelRotation = rotate * _tempRotMatrix[in][jn];
			}
		}
	}
	else if (axis == 1){
		rotate = glm::rotate(rotate, angleInDegree, glm::vec3(0.0f, 1.0f, 0.0f));
		for (int i = 0; i < this->rank; i++){
			for (int j = 0; j < this->rank; j++){
				int in, jn;
				calIndexRotate(i, j, this->rank, in, jn, angleInDegree);
				_CubeModels[i][row][j].modelTransform = _CubeModels[i][row][j].modelTranslate * rotate * _CubeModels[in][row][jn].modelRotation;
				_tempRotMatrix[i][j] = _CubeModels[i][row][j].modelRotation;

			}
		}
		for (int i = 0; i < this->rank; i++){
			for (int j = 0; j < this->rank; j++){
				int in, jn;
				calIndexRotate(i, j, this->rank, in, jn, angleInDegree);
				_CubeModels[i][row][j].modelRotation = rotate * _tempRotMatrix[in][jn];
			}
		}
	}
	else {
		rotate = glm::rotate(rotate, angleInDegree, glm::vec3(0.0f, 0.0f, 1.0f));
		for (int i = 0; i < this->rank; i++){
			for (int j = 0; j < this->rank; j++){
				int in, jn;
				calIndexRotate(i, j, this->rank, in, jn, angleInDegree);
				_CubeModels[i][j][row].modelTransform = _CubeModels[i][j][row].modelTranslate * rotate * _CubeModels[in][jn][row].modelRotation;
				_tempRotMatrix[i][j] = _CubeModels[i][j][row].modelRotation;

			}
		}
		for (int i = 0; i < this->rank; i++){
			for (int j = 0; j < this->rank; j++){
				int in, jn;
				calIndexRotate(i, j, this->rank, in, jn, angleInDegree);
				_CubeModels[i][j][row].modelRotation = rotate * _tempRotMatrix[in][jn];
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
				std::cout << "side now is " << side << std::endl;
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

void MagicCube::calIndexRotate(int i, int j, int rank, int& outI, int& outJ, float rotAng){
	glm::vec2 fromID = glm::vec2(i - (float)(rank - 1) / 2, j - (float)(rank - 1) / 2);
	glm::mat2 rot = glm::mat2(cos(rotAng), -sin(rotAng), sin(rotAng), cos(rotAng));
	fromID = rot * fromID;
	outI = std::round(fromID.x + (float)(rank - 1) / 2);
	outJ = std::round(fromID.y + (float)(rank - 1) / 2);
	//std::cout << "from: " << i << "," << j << " to " << outI << "," << outJ << endl;
	return;
}