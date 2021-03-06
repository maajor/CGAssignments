#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <glm\glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLEW
#define GLEW_STATIC
#include <gl/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../include/shader.h"

class HalfEdge;
class Vertex;
class Face;
class HalfEdgeMesh;
class HalfEdgeModel;
class Quadric;
class pairInfo;
class pairInfoLess;

class HalfEdge
{
//public:
	Vertex *start;
	Vertex *end;
	HalfEdge *prev;
	HalfEdge *pair;
	HalfEdge *next;
	Face *face;
	int index;

	friend class HalfEdgeMesh;
public:
	HalfEdge(){}
	~HalfEdge(){}
};

class Vertex{
public:
	HalfEdge *halfedge;
	glm::vec3 normal;
	glm::vec3 position;
	int index;
	int indegree;

	friend class HalfEdgeModel;
	friend class HalfEdgeMesh;
public:
	glm::vec3 getNormal() const { return normal; }
	glm::vec3 getPosition() const { return position; }
	HalfEdge* getHalfedge() const { return halfedge; }

	Vertex(){}
	~Vertex(){}

};

class Face{
//public:
	HalfEdge *halfedge;
	int index;

	friend class HalfEdgeModel;
	friend class HalfEdgeMesh;
public:
	std::vector<int> indice;
	Face(){}
	~Face(){}
	std::vector<int> getIndices() const { return indice; }
};

class pos{
public:
	float x, y, z;
	bool operator< (const pos X) const {
		return x < X.x ||
			x == X.x && (y < X.y || y == X.y && z < X.z);
	}
	
};

class Quadric{
	float r;
	glm::mat4 K;
public:

	Quadric(){
		K = glm::mat4(0);
		r = 1;
	}
	Quadric(double a, double b, double c, double d, float area){
		K = glm::mat4(
			a*a, a*b, a*c, a*d,
			a*b, b*b, b*c, b*d,
			a*c, b*c, c*c, d*c,
			a*d, b*d, c*d, d*d
			);
		r = area;
	}
	Quadric(const Quadric& Q) { *this = Q; }

	Quadric& Quadric::operator+=(const Quadric& Q)
	{
		r += Q.r;
		K += Q.K;

		return *this;
	}
	Quadric& Quadric::operator+(const Quadric& Q)
	{
		r += Q.r;
		K += Q.K;

		return *this;
	}
	
	glm::mat4 K3Homo(){
		//glm is column-major
		return glm::mat4(
			K[0][0], K[1][0], K[2][0], 0,
			K[0][1], K[1][1], K[2][1], 0,
			K[0][2], K[1][2], K[2][2], 0,
			K[0][3], K[1][3], K[2][3], 1
			);
	}
	/*
	glm::mat4 K3Homo(){
		//glm is column-major
		return glm::mat4(
			K[0][0], K[0][1], K[0][2], K[0][3],
			K[1][0], K[1][1], K[1][2], K[1][3],
			K[2][0], K[2][1], K[2][2], K[2][3],
			0, 0, 0, 1
			);
	}*/
	glm::mat4 getK(){
		return K;
	}

};

class pairInfo{
	double cost;

public:
	int vertId1, vertId2;
	glm::vec4 averageVert;
	pairInfo(){};
	pairInfo(int id1, int id2){
		vertId1 = id1; vertId2 = id2;
	}
	pairInfo(const pairInfo& pair2){ *this = pair2; }

	void caluCost(glm::vec3 pos1, glm::vec3 pos2, Quadric k1, Quadric k2){
		Quadric ksum;
		ksum = k1 + k2;
		glm::mat4 homo = ksum.K3Homo();
		float det = glm::determinant(homo);
		if (det < 1e-11 && det > -1e-11){
			glm::vec3 aver = pos1 + pos2;
			averageVert = glm::vec4(aver.x/2, aver.y/2, aver.z/2, 1);
		}
		else{
			glm::mat4 reversed = glm::inverse(homo);
			averageVert = reversed * glm::vec4(0, 0, 0, 1);
		}
		cost = glm::dot(averageVert, (ksum.getK() * averageVert));
	}

	glm::vec4 caluAverage(glm::vec3 pos1, glm::vec3 pos2, Quadric k1, Quadric k2){
		Quadric ksum;
		ksum = k1 + k2;
		glm::mat4 homo = ksum.K3Homo();
		float det = glm::determinant(homo);
		if (det < 1e-12 && det > -1e-12){
			glm::vec3 aver = pos1 + pos2;
			averageVert = glm::vec4(aver.x / 2, aver.y / 2, aver.z / 2, 1);
		}
		else{
			glm::mat4 reversed = glm::inverse(homo);
			averageVert = reversed * glm::vec4(0, 0, 0, 1);
		}
		return averageVert;
	}

	double getCost(){
		return cost;
	}

	bool operator<(const pairInfo& pair){
		return cost > pair.cost;
	}
};
/*
class pairInfoLess{
public:
	bool operator ()(const pairInfo & a, const pairInfo & b)
	{
		return a.cost>b.cost;
	}
};
*/
class HalfEdgeMesh{
	std::vector<Face*> faces;
	std::vector<Vertex*> vertices;
	std::vector<HalfEdge*> halfedges;
	std::map<std::pair<int, int>, HalfEdge*> edgeMap;

	std::vector<Quadric> verticesQuadrics;

	std::vector<float> verticesData;
	std::vector<GLuint> indicesData;

	//std::priority_queue<pairInfo,std::vector<pairInfo>, pairInfoLess> pairsHeap;
	std::vector<pairInfo> pairsHeap;

	int currentFaceNum;

	GLuint VAO, VBO, EBO;

	bool testCoOrient(int id1, int id2, int id3, int id4, glm::vec3 desiredPos){
		//std::vector<glm::vec3> neighborPos;
		std::vector<glm::vec3> towardVec;
		//std::vector<int> neighborId;

		int idnext = id3;
		HalfEdge *edgeIter = edgeMap[std::make_pair(id1, id3)];
		//neighborPos.push_back(vertices[id3]->position);

		while (idnext != id4){
			edgeIter = edgeIter->prev->pair;
			idnext = edgeIter->end->index;
			//neighborPos.push_back(vertices[idnext]->position);
			towardVec.push_back(glm::normalize(vertices[idnext]->position - desiredPos));
			//neighborId.push_back(idnext);
		}

		edgeIter = edgeMap[std::make_pair(id2, id4)];

		while (idnext != id3){
			edgeIter = edgeIter->prev->pair;
			idnext = edgeIter->end->index;
			//neighborPos.push_back(vertices[idnext]->position);
			towardVec.push_back(glm::normalize(vertices[idnext]->position - desiredPos));
			//neighborId.push_back(idnext);
		}

		assert(towardVec.size() >= 3);

		int i2, i3;
		for (int i1 = 0; i1 < towardVec.size(); i1++){
			if (i1 == towardVec.size() - 1){
				i2 = 0; i3 = 1;
			}
			else if (i1 == towardVec.size() - 2){
				i2 = i1 + 1;
				i3 = 0;
			}
			else{
				i2 = i1 + 1; i3 = i1 + 2;
			}
			//prevent sharp corner
			//if (glm::dot(towardVec[i1], towardVec[i2]) > 0.9){
			//	return false;
			//}

			glm::vec3 normal1 = glm::cross(towardVec[i1], towardVec[i2]);
			glm::vec3 normal2 = glm::cross(towardVec[i2], towardVec[i3]);

			//prevent mesh inversion
			if (glm::dot(normal1, normal2) < 0){
				return false;
			}
		}

		return true;
	}

	bool contractPair(pairInfo targetPair){
		int id1 = targetPair.vertId1;
		int id2 = targetPair.vertId2;

		if (id1 == id2)
			return false;

		HalfEdge* thisHalfEdge = edgeMap[std::make_pair(id1, id2)];
		HalfEdge* pairHalfEdge = thisHalfEdge->pair;

		int id3 = thisHalfEdge->next->end->index;
		int id4 = pairHalfEdge->next->end->index;

		//prevent mesh inversion
		if (! testCoOrient(id1, id2, id3, id4, glm::vec3(targetPair.averageVert))){
			return false;
		}

		//prevent non-manifold
		if (vertices[id1]->indegree <= 3 || vertices[id2]->indegree <= 3 || vertices[id3]->indegree <= 3 || vertices[id4]->indegree <= 3){
			return false;
		}
		vertices[id1]->indegree = vertices[id1]->indegree + vertices[id2]->indegree - 4;
		vertices[id3]->indegree--;
		vertices[id4]->indegree--;

		glm::vec3 averVertPos = glm::vec3(targetPair.averageVert);
		int idMerged = id1;
		Quadric quadricMerged;
		quadricMerged = verticesQuadrics[id1] + verticesQuadrics[id2];

		//update infos

		vertices[id1]->position = averVertPos;
		verticesQuadrics[id1] = quadricMerged;

		thisHalfEdge->next->pair->pair = thisHalfEdge->prev->pair;
		thisHalfEdge->prev->pair->pair = thisHalfEdge->next->pair;
		pairHalfEdge->next->pair->pair = pairHalfEdge->prev->pair;
		pairHalfEdge->prev->pair->pair = pairHalfEdge->next->pair;

		vertices[id1]->halfedge = thisHalfEdge->prev->pair;
		vertices[id3]->halfedge = thisHalfEdge->next->pair;
		vertices[id4]->halfedge = pairHalfEdge->next->pair;

		Face* face1 = thisHalfEdge->face;
		Face* face2 = pairHalfEdge->face;

		//makenull face1 face2 and halfedges on faces
		faces[face1->index] = NULL;
		faces[face2->index] = NULL;

		halfedges[thisHalfEdge->index] = NULL;
		halfedges[thisHalfEdge->next->index] = NULL;
		halfedges[thisHalfEdge->prev->index] = NULL;

		halfedges[pairHalfEdge->index] = NULL;
		halfedges[pairHalfEdge->next->index] = NULL;
		halfedges[pairHalfEdge->prev->index] = NULL;

		//update faces
		for (int i = 0; i < faces.size(); i++){
			if (faces[i] == NULL)
				continue;
			for (int j = 0; j < faces[i]->indice.size(); j++){
				if (faces[i]->indice[j] == id2){
					faces[i]->indice[j] = id1;
				}
			}
		}

		//update halfedges
		for (int i = 0; i < halfedges.size(); i++){
			if (halfedges[i] == NULL)
				continue;
			if (halfedges[i]->start->index == id2){
				halfedges[i]->start = vertices[id1];
			}
			if (halfedges[i]->end->index == id2){
				halfedges[i]->end = vertices[id1];
			}
		}

		//update pairHeap
		for (std::vector<pairInfo>::iterator pairIter = pairsHeap.begin(); pairIter != pairsHeap.end();){
			int ida = pairIter->vertId1;
			int idb = pairIter->vertId2;
			bool sameFlag = false;
			if (pairIter->vertId1 == id1 && pairIter->vertId2 == id2){
				pairIter = pairsHeap.erase(pairIter);
				continue;
			}
			if (ida == id2){
				//ida = id1;
				pairIter->vertId1 = id1;
				sameFlag = true;
			}
			if (idb == id2){
				//idb = id1;
				pairIter->vertId2 = id1;
				sameFlag = true;
			}
			if (sameFlag){
				pairIter->caluCost(vertices[pairIter->vertId1]->position, vertices[pairIter->vertId2]->position, verticesQuadrics[pairIter->vertId1], verticesQuadrics[pairIter->vertId2]);
			}
			pairIter++;
		}
		std::make_heap(pairsHeap.begin(), pairsHeap.end());

		//update edgemap
		std::map<std::pair<int, int>, HalfEdge*> newMap;

		edgeMap[std::make_pair(id1, id2)] = NULL;
		edgeMap[std::make_pair(id2, id1)] = NULL;

		edgeMap[std::make_pair(id3, id1)] = edgeMap[std::make_pair(id3, id2)];
		edgeMap[std::make_pair(id1, id4)] = edgeMap[std::make_pair(id2, id4)];

		edgeMap[std::make_pair(id2, id3)] = NULL;
		edgeMap[std::make_pair(id2, id4)] = NULL;
		edgeMap[std::make_pair(id4, id2)] = NULL;
		edgeMap[std::make_pair(id3, id2)] = NULL;

		std::map<std::pair<int, int>, HalfEdge*>::iterator it;
		for (it = edgeMap.begin(); it != edgeMap.end();)
		{
			if (it->second == NULL){
				it = edgeMap.erase(it);
				continue;
			}
			if (it->first.first == id2 && it->first.second == id2){
				//newMap[std::make_pair(id1, id1)] = it->second;
				it = edgeMap.erase(it);
				continue;
			}
			if (it->first.first == id2 && it->first.second != id3){
				newMap[std::make_pair(id1, it->first.second)] = it->second;
				it = edgeMap.erase(it);
				continue;
			}
			if (it->first.second == id2 && it->first.first != id4){
				newMap[std::make_pair(it->first.first, id1)] = it->second;
				it = edgeMap.erase(it);
				continue;
			}
			it++;
		}

		std::map<std::pair<int, int>, HalfEdge*>::iterator itNew;
		for (itNew = newMap.begin(); itNew != newMap.end(); itNew++)
		{
			edgeMap[itNew->first] = itNew->second;
		}

		//clean up useless edge and face
		delete(thisHalfEdge->next);
		delete(thisHalfEdge->prev);
		delete(thisHalfEdge);

		delete(pairHalfEdge->next);
		delete(pairHalfEdge->prev);
		delete(pairHalfEdge);

		delete(face1);
		delete(face2);


		//clean up vertex
		Vertex* oldVert = vertices[id2];
		delete(oldVert);
		vertices[id2] = NULL;
		
		return true;
		//do not clean verticesQuadrics, which should be cleaned after whole simplification

	}

	void resetMeshData(){
		std::map<int, int> remapVert;
		std::vector<Vertex*> newVets;
		std::vector <Face*> newFaces;

		for (int i = 0; i < vertices.size(); i++){
			if (vertices[i] != NULL){
				newVets.push_back(vertices[i]);
				newVets.back()->index = newVets.size() - 1;
				newVets.back()->normal = glm::vec3(0);
				remapVert[i] = newVets.size() - 1;
			}
		}

		for (int i = 0; i < faces.size(); i++){
			if (faces[i] != NULL){
				faces[i]->index = newFaces.size();
				for (int j = 0; j < faces[i]->indice.size(); j++){
					faces[i]->indice[j] = remapVert[faces[i]->indice[j]];
				}
				newFaces.push_back(faces[i]);

				//calculate and add face normal to each vertex
				glm::vec3 normal = glm::cross(
					glm::vec3(newVets[faces[i]->indice[1]]->position - newVets[faces[i]->indice[0]]->position),
					glm::vec3(newVets[faces[i]->indice[2]]->position - newVets[faces[i]->indice[0]]->position)
					);
				normal = glm::normalize(normal);
				newVets[faces[i]->indice[0]]->normal += normal;
				newVets[faces[i]->indice[1]]->normal += normal;
				newVets[faces[i]->indice[2]]->normal += normal;
			}
		}

		//normalize each normal
		for (int i = 0; i < newVets.size(); i++){
			newVets[i]->normal = glm::normalize(newVets[i]->normal);
		}

		*this = HalfEdgeMesh(newVets, newFaces);

	}


public:
	int targetFaceNum;


	HalfEdgeMesh(){}
	HalfEdgeMesh(std::vector<Vertex*> vets, std::vector<Face*> facs){
		edgeMap.clear();
		halfedges.clear();
		faces = facs;
		vertices = vets;
		currentFaceNum = facs.size();
		targetFaceNum = INT_MAX;

		//loop faces to set edges and faces
		for (int i = 0; i < faces.size(); i++){
			int vetSize = faces[i]->indice.size();
			int thisVet, nextVet, thisEdg, nextEdg, prvEdg;
			int offsetSize = halfedges.size();
			//set start and end point and face for one halfedge
			for (int j = 0; j < vetSize; j++){
				if (j == vetSize - 1)
					nextVet = 0;
				else
					nextVet = j + 1;
				thisVet = j;
				HalfEdge *thisHalfEdge = new HalfEdge();
				thisHalfEdge->index = offsetSize + j;
				thisHalfEdge->start = vertices[faces[i]->indice[thisVet]];
				vertices[faces[i]->indice[thisVet]]->indegree++;
				thisHalfEdge->end = vertices[faces[i]->indice[nextVet]];
				thisHalfEdge->face = faces[i];
				halfedges.push_back(thisHalfEdge);
				vertices[faces[i]->indice[thisVet]]->halfedge = thisHalfEdge;

				edgeMap[std::make_pair(faces[i]->indice[thisVet], faces[i]->indice[nextVet])] = thisHalfEdge;
				HalfEdge *pairEdge;
				if (edgeMap.count(std::make_pair(faces[i]->indice[nextVet], faces[i]->indice[thisVet]))) {
					pairEdge = edgeMap[std::make_pair(faces[i]->indice[nextVet], faces[i]->indice[thisVet])];
					pairEdge->pair = thisHalfEdge;
					thisHalfEdge->pair = pairEdge;
				}
			}
			//set previous and next edge for one halfedge
			for (int j = 0; j < vetSize; j++){
				if (j == vetSize - 1)
					nextEdg = offsetSize;
				else
					nextEdg = offsetSize + j + 1;
				if (j == 0)
					prvEdg = halfedges.size() - 1;
				else
					prvEdg = offsetSize + j - 1;
				thisEdg = offsetSize + j;
				halfedges[thisEdg]->next = halfedges[nextEdg];
				halfedges[thisEdg]->prev = halfedges[prvEdg];
			}
			faces[i]->halfedge = halfedges[offsetSize];
		}
		setupGLData();
	}
	~HalfEdgeMesh(){}

	void setupGLData(){

		for (int i = 0; i < vertices.size(); i++){
			verticesData.push_back(vertices[i]->position.x);
			verticesData.push_back(vertices[i]->position.y);
			verticesData.push_back(vertices[i]->position.z);
			verticesData.push_back(vertices[i]->normal.x);
			verticesData.push_back(vertices[i]->normal.y);
			verticesData.push_back(vertices[i]->normal.z);
		}	

		for (int i = 0; i < faces.size(); i++){
			for (int j = 0; j < faces[i]->indice.size(); j++){
				indicesData.push_back(faces[i]->indice[j]);
			}
		}

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, this->verticesData.size() * sizeof(float), &this->verticesData[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indicesData.size() * sizeof(GLuint), &this->indicesData[0], GL_STATIC_DRAW);

		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

		glBindVertexArray(0);
	}

	std::vector<Face*> getFaces() const { return faces; }
	std::vector<Vertex*> getVertices() const { return vertices; }
	std::vector<HalfEdge*> getHalfedges() const { return halfedges; }

	void Draw(Shader shader){
		glm::mat4 model = glm::mat4();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, this->indicesData.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void quadricSimplify(int times){
		collect_quadrics();

		//To Do.
		//Constraint Boundaries!

		collect_pairs();

		while (times > 0 && pairsHeap.size() > 0 && currentFaceNum > 4 && currentFaceNum > targetFaceNum){
			pairInfo currentPair;
			std::pop_heap(pairsHeap.begin(), pairsHeap.end());
			currentPair = pairsHeap.back();
			pairsHeap.pop_back();
			if (contractPair(currentPair)){
				currentFaceNum = currentFaceNum - 2;
				times--;
			}
		}
		resetMeshData();
	}

	void quadricSimplifyTo(int targetFace){
		targetFaceNum = targetFace;

		while (currentFaceNum > targetFaceNum){
			quadricSimplify(5000);
			targetFaceNum = targetFace;
		}
	}

	void collect_quadrics(){
		pairsHeap.empty();
		//std::make_heap(pairsHeap.begin(), pairsHeap.end());

		verticesQuadrics.clear();
		for (int i = 0; i < vertices.size(); i++){
			verticesQuadrics.push_back(Quadric());
		}
		
		for (int i = 0; i < faces.size(); i++){
			std::vector<int> f = faces[i]->getIndices();
			glm::vec4 plane = calcPlane(vertices[f[0]]->position, vertices[f[1]]->position, vertices[f[2]]->position);
			verticesQuadrics[f[0]] += Quadric(plane.x, plane.y, plane.z, plane.w, 1);
			verticesQuadrics[f[1]] += Quadric(plane.x, plane.y, plane.z, plane.w, 1);
			verticesQuadrics[f[2]] += Quadric(plane.x, plane.y, plane.z, plane.w, 1);
		}
	}

	void collect_pairs(){
		for (int i = 0; i < halfedges.size(); i++){
			int id1 = halfedges[i]->start->index;
			int id2 = halfedges[i]->end->index;
			if (id1 > id2){
				pairInfo thispair(id1, id2);
				thispair.caluCost(vertices[id1]->position, vertices[id2]->position, verticesQuadrics[id1], verticesQuadrics[id2]);
				pairsHeap.push_back(thispair);
				std::push_heap(pairsHeap.begin(), pairsHeap.end());
			}
		}
		std::make_heap(pairsHeap.begin(), pairsHeap.end());
	}

	glm::vec4 calcPlane(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3){
		glm::vec3 normal = glm::cross(v2 - v1, v3 - v1);
		normal = glm::normalize(normal);
		return glm::vec4(normal, -glm::dot(normal, v1));
	}

	int vertexSize(){
		return vertices.size();
	}

	int faceSize(){
		return faces.size();
	}

	void printIndegree(){
		for (int i = 0; i < vertices.size(); i++){
			if (vertices[i] != NULL ){
				std::cout << "vet " << i << "has indegree" << vertices[i]->indegree << std::endl;
			}
		}
	}

	void saveMesh(){
		std::ofstream file("save.obj");

		for (int i = 0; i < vertices.size(); i++)
		{
			file << "v " << vertices[i]->position.x << " " << vertices[i]->position.y << " " << vertices[i]->position.z << std::endl;

		}
		// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (int i = 0; i < faces.size(); i++)
		{
			file << "f";
			for (int j = 0; j < faces[i]->indice.size(); j++)
				file << " " << faces[i]->indice[j] + 1;
			file << std::endl;
		}
		file.close();
	}

};

class HalfEdgeModel{
public:
	std::vector<HalfEdgeMesh> meshes;
	glm::mat4 modelTransform;
	std::string directory;

public:
	HalfEdgeModel(std::string const &path)
	{
		this->loadModel(path);
		modelTransform = glm::mat4();
	}
	HalfEdgeModel(){}

	void quadricSimplify(double percentToPreserve){
		if (percentToPreserve > 1)
			percentToPreserve = 1;
		if (percentToPreserve < 0)
			percentToPreserve = 0.01;
		for (int i = 0; i < this->meshes.size(); i++){
			int totalFace = this->meshes[i].faceSize();
			this->meshes[i].quadricSimplifyTo(totalFace*percentToPreserve);
		}

	}

	void Draw(Shader shader)
	{
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelTransform));
		for (GLuint i = 0; i < this->meshes.size(); i++)
			this->meshes[i].Draw(shader);
	}

	void loadModel(std::string path)
	{
		// Read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_FixInfacingNormals  | aiProcess_Triangulate  | aiProcess_GenSmoothNormals);
		// Check for errors
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}
		// Retrieve the directory path of the filepath
		this->directory = path.substr(0, path.find_last_of('/'));

		// Process ASSIMP's root node recursively
		this->processNode(scene->mRootNode, scene);
	}

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene)
	{
		// Process each mesh located at the current node
		for (GLuint i = 0; i < node->mNumMeshes; i++)
		{
			// The node object only contains indices to index the actual objects in the scene. 
			// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			
			this->meshes.push_back(this->processMesh(mesh, scene));
		}
		// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			this->processNode(node->mChildren[i], scene);
		}

	}

	void saveModel(aiMesh* mesh){

		std::ofstream file("save.obj");

		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			file << "v " << mesh->mVertices[i].x << " " << mesh->mVertices[i].y << " " << mesh->mVertices[i].z << std::endl;
		
		}
		// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			file << "f";
			for (GLuint j = 0; j < face.mNumIndices; j++)
				file << " " << face.mIndices[j] + 1;
			file << std::endl;
		}
		file.close();
	}

	HalfEdgeMesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// Data to fill
		std::vector<Vertex*> vertices;
		std::vector<Face*> faces;

		// Walk through each of the mesh's vertices
		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex *vertex = new Vertex();
			glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// Positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex->position = vector;
			// Normals
			if (mesh->HasNormals()){
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex->normal = vector;
			}
			vertex->index = i;
			vertex->indegree = 0;
			// Texture Coordinates
			vertices.push_back(vertex);
		}
		// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			Face *meshface = new Face();
			// Retrieve all indices of the face and store them in the indices vector
			for (GLuint j = 0; j < face.mNumIndices; j++)
				meshface->indice.push_back(face.mIndices[j]);
			meshface->index = i;
			faces.push_back(meshface);
		}
		// Return a mesh object created from the extracted mesh data
		return HalfEdgeMesh(vertices, faces);
	}
};
