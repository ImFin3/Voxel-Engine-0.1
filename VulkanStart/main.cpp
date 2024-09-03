#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "VoxelEngine.h"
#include "VoxelFramework.h"
#include "Randomizer.h"
#include <cmath>

// Change bool Value to switch from Rasterizer to Ray tracer
// VoxelFramework inherits from  VoxelEngine (The Core) | VoxelFramework can be used to change singular Functions => I used it for Voxel Generation testing purposes
// shader.vert and shader.frag are Shaders from Rasterizer approach | shader.comp, compshader.vert and compshader.frag are for the Ray tracing approach

// Inputs and Makros
// Mouse Inputs turn the Camera,
// WASD moves the Camera through the Scene | SPACE and Left CONTROL are used to go UP and DOWN in the Scene
// "u" can be used to update the Vertex and Index Buffer from a simple colourfull plane to the desired Voxel Mass created in VoxelFramework::InitSceneObjects (Rasterizer Only)


int main() { 

    bool rayTracing = false;

    VoxelFramework* app = new VoxelFramework(rayTracing);

    try {
        if (app) 
        {
            app->run();
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }   
    if (app) 
    {
        delete app;
        app = nullptr;
    }

    return EXIT_SUCCESS;
}