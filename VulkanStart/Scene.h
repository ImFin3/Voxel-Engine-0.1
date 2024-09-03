#ifndef SCENE_H
#define SCENE_H

#include "MyStructs.h"
#include "Voxel.h"
#include "Camera.h"
#include <ctime>
#include "Randomizer.h"
#include <thread>

const int NUMBER_OF_THREADS = 3;


class Scene
{
private:
	Camera m_Camera;
	std::vector<Voxel> m_voxel;

public:
	Scene();
	Scene(Camera& a_camera);

	Camera& GetCamera();
	std::vector<Voxel> GetVoxel();

	void SetVoxel(const std::vector<Voxel>& a_voxel);
	void AddVoxel(const Voxel& a_voxel);
	void AddVoxel(const std::vector<Voxel>& a_voxel);

	void OverwriteVertsAndIndices(std::vector<Vertex>& a_vertices, std::vector<uint32_t>& a_indices);
	void OverwriteVertsAndIndicesMT(std::vector<Vertex>& a_vertices, std::vector<uint32_t>& a_indices);
	void AddVertsAndIndices(std::vector<Vertex>& a_vertices, std::vector<uint32_t>& a_indices);

	void GenerateRandomVoxelMass(int a_voxelCount, const glm::vec3& a_start, const glm::vec3& a_end, const float& a_size);
	
};
void DoWork(std::vector<Vertex>* a_vertices, std::vector<uint32_t>* a_indices, const int& a_start, const int& a_end, std::vector<Voxel>&& a_voxel);
#endif // !SCENE_H

