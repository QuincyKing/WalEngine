#pragma once

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../component/Material.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>
using namespace std;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	//glm::vec3 Tangent;
	//glm::vec3 Bitangent;
};

struct Tex
{
	unsigned int id;
	string type;
	string path;
};

class Mesh
{
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Tex> textures;
	unsigned int VAO;

	Mesh()
	{

	}

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Tex> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		setup_mesh();
	}

	void draw()
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		//for (unsigned int i = 0; i < textures.size(); i++)
		//{
		//	glActiveTexture(GL_TEXTURE0 + i);
		//	string number;
		//	string name = textures[i].type;
		//	if (name == "texture_diffuse")
		//		number = std::to_string(diffuseNr++);
		//	else if (name == "texture_specular")
		//		number = std::to_string(specularNr++); // transfer unsigned int to stream
		//	else if (name == "texture_normal")
		//		number = std::to_string(normalNr++); // transfer unsigned int to stream
		//	else if (name == "texture_height")
		//		number = std::to_string(heightNr++); // transfer unsigned int to stream

		//	shader->set_int((name + number), i);

		//	glBindTexture(GL_TEXTURE_2D, textures[i].id);
		//}

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}
	//void CalcNormals();
	//void CalcTangents();

private:
	unsigned int VBO, EBO;
	void setup_mesh();
};