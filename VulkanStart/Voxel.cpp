#include "Voxel.h"

Voxel::Voxel(const glm::vec3& a_position, const glm::vec3& a_color, const float& a_size)
{
	m_position = a_position;
	m_color = a_color;
	m_size = a_size;
}

glm::vec3 Voxel::GetPosition()
{
	return m_position;
}

glm::vec3 Voxel::GetColor()
{
	return m_color;
}

float Voxel::GetSize()
{
	return m_size;
}

std::vector<Vertex> Voxel::GetVertices()
{
	return 
	{
		{ m_position - glm::vec3(-m_size, -m_size, -m_size), m_color},
		{ m_position - glm::vec3(m_size , -m_size, -m_size), m_color},
		{ m_position - glm::vec3(m_size ,  m_size, -m_size), m_color},
		{ m_position - glm::vec3(-m_size,  m_size, -m_size), m_color},

		{ m_position - glm::vec3(-m_size, -m_size,  m_size), m_color},
		{ m_position - glm::vec3(m_size , -m_size,  m_size), m_color},
		{ m_position - glm::vec3(m_size ,  m_size,  m_size), m_color},
		{ m_position - glm::vec3(-m_size,  m_size,  m_size), m_color}
	};
}

std::vector<uint32_t> Voxel::GetIndices()
{
	return 
	{
		0, 1, 2, 2, 3, 0,	//Top
		4, 6, 5, 4, 7, 6,	//Bottom
		1, 5, 6, 1, 6, 2,	//Front
		2, 6, 7, 2, 7, 3,	//Right
		0, 4, 5, 0, 5, 1,	//Left
		3, 7, 4, 3, 4, 0	//Back
	};
}
