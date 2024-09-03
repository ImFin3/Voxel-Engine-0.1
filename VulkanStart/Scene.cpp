#include "Scene.h"

Scene::Scene()
{

}

Scene::Scene(Camera& a_camera)
{
	m_Camera = a_camera;
	m_voxel = {};
}

Camera& Scene::GetCamera()
{
	return m_Camera;
}

std::vector<Voxel> Scene::GetVoxel()
{
	return m_voxel;
}

void Scene::SetVoxel(const std::vector<Voxel>& a_voxel)
{
	m_voxel = a_voxel;
}

void Scene::AddVoxel(const Voxel& a_voxel)
{
	m_voxel.push_back(a_voxel);
}

void Scene::AddVoxel(const std::vector<Voxel>& a_voxel)
{
	int voxelCount = a_voxel.size();

	for (int i = 0; i < voxelCount; i++) {
		m_voxel.push_back(a_voxel.at(i)); 
	}
}

void Scene::OverwriteVertsAndIndices(std::vector<Vertex>& a_vertices, std::vector<uint32_t>& a_indices)
{
	a_vertices.clear();
	a_indices.clear();
	int voxelCount = m_voxel.size();
	
	for (int i = 0; i < voxelCount; i++) {
		uint32_t vertexOffset = a_vertices.size();

		std::vector<Vertex> vertices = m_voxel.at(i).GetVertices();
		std::vector<uint32_t> indices = Voxel::GetIndices();
		for (int j = 0; j < VERTEX_COUNT_PER_VOXEL; j++) {
			a_vertices.emplace_back(vertices.at(j));
		}
		for (int k = 0; k < INDICES_COUNT_PER_VOXEL; k++) {
			a_indices.emplace_back(indices.at(k) + vertexOffset);
		}
	}
}

void Scene::OverwriteVertsAndIndicesMT(std::vector<Vertex>& a_vertices, std::vector<uint32_t>& a_indices)
{
	a_vertices.clear();
	a_indices.clear();
	float voxelCount = m_voxel.size();

	int voxelPerThreadGroup = std::ceil(voxelCount / NUMBER_OF_THREADS);
	
	std::vector<std::thread> threads;
	std::vector<std::vector<Vertex>*> vertexValues;
	std::vector<std::vector<uint32_t>*> indicesValues;


	for (int i = 0; i < NUMBER_OF_THREADS; i++)
	{
		int start = i * voxelPerThreadGroup;
		int end;

		if (voxelCount - start > voxelPerThreadGroup) 
		{ 
			end = start + voxelPerThreadGroup - 1; 
		} 
		else
		{ 
			end = voxelCount - 1;
		}

		vertexValues.emplace_back(new std::vector<Vertex>);
		indicesValues.emplace_back(new std::vector<uint32_t>);
		threads.emplace_back(DoWork, vertexValues.at(i), indicesValues.at(i), start, end, m_voxel);
	}

	for (int j = 0; j < NUMBER_OF_THREADS; j++) 
	{
		threads.at(j).join();
		a_vertices.insert(a_vertices.end(), vertexValues.at(j)->begin(), vertexValues.at(j)->end());
		a_indices.insert(a_indices.end(), indicesValues.at(j)->begin(), indicesValues.at(j)->end());
	}
	
}

void Scene::AddVertsAndIndices(std::vector<Vertex>& a_vertices, std::vector<uint32_t>& a_indices)
{
	int voxelCount = m_voxel.size();

	for (int i = 0; i < voxelCount; i++) {
		uint32_t vertexOffset = a_vertices.size();

		std::vector<Vertex> vertices = m_voxel.at(i).GetVertices();
		std::vector<uint32_t> indices = Voxel::GetIndices();
		for (int j = 0; j < VERTEX_COUNT_PER_VOXEL; j++) {
			a_vertices.emplace_back(vertices.at(j));
		}
		for (int k = 0; k < INDICES_COUNT_PER_VOXEL; k++) {
			a_indices.emplace_back(indices.at(k) + vertexOffset);
		}
	}
}


void Scene::GenerateRandomVoxelMass(int a_voxelCount, const glm::vec3& a_start, const glm::vec3& a_end, const float& a_size)
{
	srand(std::time(nullptr));

	for (a_voxelCount; a_voxelCount > 0; a_voxelCount--) {
		glm::vec3 pos;
		pos.x = Randomizer::RandomIntAsFloatBetween(a_start.x, a_end.x);
		pos.y = Randomizer::RandomIntAsFloatBetween(a_start.y, a_end.y);
		pos.z = Randomizer::RandomIntAsFloatBetween(a_start.z, a_end.z);

		glm::vec3 col;
		col.x = Randomizer::RandomFloatBetween01();
		col.y =	Randomizer::RandomFloatBetween01();
		col.z = Randomizer::RandomFloatBetween01();

		AddVoxel(Voxel(pos, col, a_size)); 
	}
}

void DoWork(std::vector<Vertex>* a_vertices, std::vector<uint32_t>* a_indices, const int& a_start, const int& a_end, std::vector<Voxel>&& a_voxel)
{

	for (int i = a_start; i <= a_end; i++)
	{
		uint32_t verticesVoxelOffset = i * VERTEX_COUNT_PER_VOXEL;
		uint32_t indicesVoxelOffset = i * INDICES_COUNT_PER_VOXEL;

		std::vector<Vertex> vertices = a_voxel.at(i).GetVertices();
		std::vector<uint32_t> indices = Voxel::GetIndices();

		for (int j = 0; j < VERTEX_COUNT_PER_VOXEL; j++)
		{
			a_vertices->emplace_back(vertices.at(j));
		}
		for (int k = 0; k < INDICES_COUNT_PER_VOXEL; k++)
		{
			a_indices->emplace_back(indices.at(k) + verticesVoxelOffset);
		}
	}
}
