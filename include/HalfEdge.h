#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <map>
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
//public:
	HalfEdge *halfedge;
	glm::vec3 normal;
	glm::vec3 position;
	int index;

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
	std::vector<int> indice;
	int index;

	friend class HalfEdgeModel;
	friend class HalfEdgeMesh;
public:
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

class HalfEdgeMesh{
	std::vector<Face*> faces;
	std::vector<Vertex*> vertices;
	std::vector<HalfEdge*> halfedges;
	std::map<std::pair<pos, pos>, HalfEdge*> edgeMap;

	std::vector<float> verticesData;
	std::vector<GLuint> indicesData;
	GLuint VAO, VBO, EBO;

public:
	HalfEdgeMesh(){}
	HalfEdgeMesh(std::vector<Vertex*> vets, std::vector<Face*> facs){
		edgeMap.clear();
		halfedges.clear();
		faces = facs;
		vertices = vets;

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
				thisHalfEdge->end = vertices[faces[i]->indice[nextVet]];
				thisHalfEdge->face = faces[i];
				halfedges.push_back(thisHalfEdge);
				vertices[faces[i]->indice[thisVet]]->halfedge = thisHalfEdge;

				pos startPos;
				startPos.x = thisHalfEdge->start->position.x;
				startPos.y = thisHalfEdge->start->position.y;
				startPos.z = thisHalfEdge->start->position.z;
				pos endPos;
				endPos.x = thisHalfEdge->end->position.x;
				endPos.y = thisHalfEdge->end->position.y;
				endPos.z = thisHalfEdge->end->position.z;
				//find pairedge
				edgeMap[std::make_pair(startPos, endPos)] = thisHalfEdge;
				HalfEdge *pairEdge;
				if (edgeMap.count(std::make_pair(endPos, startPos))) {
					pairEdge = edgeMap[std::make_pair(endPos, startPos)];
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
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
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
		//saveModel(mesh);
		// Return a mesh object created from the extracted mesh data
		return HalfEdgeMesh(vertices, faces);
	}
};
