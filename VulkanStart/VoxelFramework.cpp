#include "VoxelFramework.h"

VoxelFramework::VoxelFramework(bool a_compute)
{
	m_useCompute = a_compute;
}

void VoxelFramework::InitSceneObjects()
{
	// Objects can be initialized in this function

	m_scenes.at(m_currentScene).AddVoxel(Voxel(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 1.0f));

	m_scenes.at(m_currentScene).GenerateRandomVoxelMass(1000000, glm::vec3(0, 0, 0), glm::vec3(300, 300, 300), 0.2f);
}