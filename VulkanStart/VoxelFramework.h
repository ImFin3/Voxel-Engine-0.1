#ifndef VOXEL_FRAMEWORK_H

#define VOXEL_FRAMEWORK_H

#include "VoxelEngine.h";

class VoxelFramework : public VoxelEngine{

public:
	VoxelFramework(bool a_compute);

	void InitSceneObjects();
};

#endif // !VOXEL_FRAMEWORK_H
