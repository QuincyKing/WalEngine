#include "mesh.h"

void Mesh::CalcNormals()
{
	/*m_normals.clear();
	m_normals.reserve(m_positions.size());

	for (unsigned int i = 0; i < m_positions.size(); i++)
		m_normals.push_back(glm::vec3(0, 0, 0));

	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		int i0 = indices[i];
		int i1 = indices[i + 1];
		int i2 = indices[i + 2];

		glm::vec3 v1 = m_positions[i1] - m_positions[i0];
		glm::vec3 v2 = m_positions[i2] - m_positions[i0];

		glm::vec3 normal = glm::normalize(cross(v1, v2));

		m_normals[i0] = m_normals[i0] + normal;
		m_normals[i1] = m_normals[i1] + normal;
		m_normals[i2] = m_normals[i2] + normal;
	}

	for (unsigned int i = 0; i < m_normals.size(); i++)
		m_normals[i] = m_normals[i].Normalized();*/
}

void Mesh::CalcTangents()
{
	/*m_tangents.clear();
	m_tangents.reserve(m_positions.size());

	for (unsigned int i = 0; i < m_positions.size(); i++)
		m_tangents.push_back(glm::vec3(0, 0, 0));

	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		int i0 = indices[i];
		int i1 = indices[i + 1];
		int i2 = indices[i + 2];

		glm::vec3 edge1 = m_positions[i1] - m_positions[i0];
		glm::vec3 edge2 = m_positions[i2] - m_positions[i0];

		float deltaU1 = m_texCoords[i1].GetX() - m_texCoords[i0].GetX();
		float deltaU2 = m_texCoords[i2].GetX() - m_texCoords[i0].GetX();
		float deltaV1 = m_texCoords[i1].GetY() - m_texCoords[i0].GetY();
		float deltaV2 = m_texCoords[i2].GetY() - m_texCoords[i0].GetY();

		float dividend = (deltaU1 * deltaV2 - deltaU2 * deltaV1);
		float f = dividend == 0.0f ? 0.0f : 1.0f / dividend;

		glm::vec3 tangent = glm::vec3(0, 0, 0);

		tangent.SetX(f * (deltaV2 * edge1.GetX() - deltaV1 * edge2.GetX()));
		tangent.SetY(f * (deltaV2 * edge1.GetY() - deltaV1 * edge2.GetY()));
		tangent.SetZ(f * (deltaV2 * edge1.GetZ() - deltaV1 * edge2.GetZ()));


		m_tangents[i0] += tangent;
		m_tangents[i1] += tangent;
		m_tangents[i2] += tangent;
	}

	for (unsigned int i = 0; i < m_tangents.size(); i++)
		m_tangents[i] = m_tangents[i].Normalized();*/
}

void Mesh::setup_mesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}