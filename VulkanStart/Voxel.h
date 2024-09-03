#ifndef VOXEL_H
#define VOXEL_H

#include <glm/glm.hpp>
#include <array>
#include "MyStructs.h"
#include <iostream>

const int VERTEX_COUNT_PER_VOXEL = 8;
const int INDICES_COUNT_PER_VOXEL = 36;

class Voxel 
{
private:
	glm::vec3 m_position;
	glm::vec3 m_color;
	float m_size;

public:
	/*Voxel(const Voxel& voxel)
		: m_position(voxel.m_position), m_color(voxel.m_color), m_size(voxel.m_size)
	{
		std::cout << "Copied!" << std::endl;
	}*/

	Voxel(const glm::vec3& a_position, const glm::vec3& a_color, const float& a_size);

	glm::vec3 GetPosition();
	glm::vec3 GetColor();
	float GetSize();

	std::vector<Vertex>GetVertices();
	static std::vector<uint32_t>GetIndices();
};
#endif // !VOXEL_H

