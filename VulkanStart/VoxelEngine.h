#ifndef HELLO_TRIANGLE_APPLICATION_H

#define HELLO_TRIANGLE_APPLICATION_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#define VK_ENABLE_BETA_EXTENSIONS 

#include <vulkan/vulkan.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>	// Necessary for std::clamp
#include <vector>
#include <cstring>
#include <cstdlib>		// Necessary for uint32_t
#include <cstdint>
#include <limits>		// Necessary for std::numeric_limits
#include <array>
#include <set>

#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define GLM_FORCE_RADIANS 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>


#pragma region MyIncludes

#include "UserInput.h";
#include "Camera.h"
#include "Voxel.h"
#include "MyStructs.h"
#include "Scene.h"


#pragma endregion

const std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const int WIDTH = 1920;
const int HEIGHT = 1080;

const int PARTICLE_COUNT = 5;

const float RED = 0.35f;
const float GREEN = 0.0f;
const float BLUE = 1.0f;
const float ALPHA = 1.0f;

const std::vector<VkQueueFlagBits> neededQueueFlags = { VK_QUEUE_GRAPHICS_BIT };

class VoxelEngine
{
public:
	virtual void run();
	bool framebufferResized = false;  

protected:
	
	bool m_useCompute = false;

#pragma region VulkanBase

	void initWindow();
	void initVulkan();
	void initVulkanCompute();
	void mainLoop();
	void cleanup();


	void processInput();

	void createAndExecuteShaderBat();

	void createInstance();
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();

	void setupDebugMessenger();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT a_messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT a_messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* a_pCallbackData,
		void* a_pUserData);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& a_createInfo);
	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance a_instance,
		const VkDebugUtilsMessengerCreateInfoEXT* a_pCreateInfo,
		const VkAllocationCallbacks* a_pAllocator,
		VkDebugUtilsMessengerEXT* a_pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(
		VkInstance a_instance,
		VkDebugUtilsMessengerEXT a_debugMessenger,
		const VkAllocationCallbacks* a_pAllocator);

	void pickPhysicalDevice();
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice a_device, bool a_debugQueueFamilies);
	int rateDeviceSuitability(VkPhysicalDevice a_device);
	bool isDeviceSuitable(VkPhysicalDevice a_device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice a_device);

	void createLogicalDevice();

	void createSurface();

	void createSwapChain();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice a_device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& a_availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& a_availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& a_capabilities);

	void createImageViews();

	void createRenderPass();

	void createDescriptorLayout();

	void createGraphicsPipeline();
	VkShaderModule createShaderModule(const std::vector<char>& a_code);

	void createFramebuffers();

	void createCommandPool();

	void createVertexBuffer();
	uint32_t findMemoryType(uint32_t a_typeFilter, VkMemoryPropertyFlags a_properties);
	void createBuffer(VkDeviceSize a_size, VkBufferUsageFlags a_usage, VkMemoryPropertyFlags a_properties, VkBuffer& a_buffer, VkDeviceMemory& a_bufferMemory);
	void copyBuffer(VkBuffer a_srcBuffer, VkBuffer a_dstBuffer, VkDeviceSize a_size);

	void createIndexBuffer(); 

	void createUniformBuffers();

	void createDescriptorPool();
	void createDescriptorSets();

	void createCommandBuffers();

	void createSyncObjects();

	void recordCommandBuffer(VkCommandBuffer a_commandBuffer, uint32_t a_imageIndex, std::vector<VkDescriptorSet> a_descriptorSets);

	void drawFrame();
	void updateUniformBuffer(uint32_t a_currentImage);

	void recreateSwapchain();
	void cleanupSwapchain();

	void createDepthResources();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& a_candidates, VkImageTiling a_tiling, VkFormatFeatureFlags a_features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat a_format); 
	void createImage(uint32_t a_width, uint32_t a_height, VkFormat a_format, VkImageTiling a_tiling, VkImageUsageFlags a_usage, 
		VkMemoryPropertyFlags a_properties, VkImage& a_image, VkDeviceMemory& a_imageMemory, VkImageLayout a_initialLayout);
	VkImageView createImageView(VkImage a_image, VkFormat a_format, VkImageAspectFlags a_aspectFlags);
	void transitionImageLayout(VkImage a_image, VkFormat a_format, VkImageLayout a_oldLayout, VkImageLayout a_newLayout); 
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer a_commandBuffer);

	void updateBuffers();

	std::vector<Vertex> m_vertices = {
		{{-0.5f, -0.5f, 0.0f},	{1.0f, 0.0f, 0.0f}},	//ROT
		{{0.5f, -0.5f, 0.0f},	{0.0f, 1.0f, 0.0f}},	//GRÜN
		{{0.5f, 0.5f, 0.0f},	{0.0f, 0.0f, 1.0f}},	//BLAU
		{{-0.5f, 0.5f, 0.0f},	{1.0f, 1.0f, 1.0f}},	//WEIß
	};

	std::vector<uint32_t> m_indices = {
		0, 1, 2, 2, 3, 0
	};

	GLFWwindow* m_pWindow = nullptr;
	VkInstance m_instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_logicalDevice = VK_NULL_HANDLE;
	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	VkQueue m_presentQueue = VK_NULL_HANDLE;
	VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	std::vector<VkImageView> m_swapChainImageViews;
	VkRenderPass m_renderPass = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> m_swapChainFramebuffers;
	VkCommandPool m_commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_commandBuffers;
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;
	VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory m_vertexBufferMemory = VK_NULL_HANDLE;
	VkBuffer m_indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory m_indexBufferMemory = VK_NULL_HANDLE;
	std::vector<VkBuffer> m_uniformBuffers;
	std::vector<VkDeviceMemory> m_uniformBuffersMemory; 
	std::vector<void*> m_uniformBuffersMapped;
	VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> m_descriptorSets; 
	int m_currentFrame = 0;
	VkImage m_depthImage = VK_NULL_HANDLE;
	VkDeviceMemory m_depthImageMemory = VK_NULL_HANDLE;
	VkImageView m_depthImageView = VK_NULL_HANDLE;

#pragma endregion


#pragma region Raycasting

	//init
	void createRenderPassCompute();
	void createDescriptorLayoutCompute();
	void createGraphicsAndComputePipeline();
	void createFramebuffersCompute();
	void createShaderStorageBuffersCompute();
	void createDescriptorPoolCompute();
	void createDescriptorSetsCompute();
	void createCommandBuffersCompute();


	//mainLoop
	void drawFrameCompute(); 
	void recordCommandBufferCompute(VkCommandBuffer a_commandBuffer);


	std::vector<VkFence> m_computeInFlightFences;
	std::vector<VkSemaphore> m_computeFinishedSemaphores;
	//VertexBuffer
	//IndexBuffer
	VkBuffer m_voxelBuffer;
	VkDeviceMemory m_voxelBufferMemory;
	//UniformBuffer
	VkQueue m_queueCompute;
	std::vector<VkDescriptorSet> m_descriptorSetsCompute;
	VkDescriptorSetLayout m_descriptorSetLayoutCompute;
	VkPipelineLayout m_pipelineLayoutCompute;
	VkPipeline m_pipelineCompute;
	//graphicsPipeline
	std::vector<VkCommandBuffer> m_commandBuffersCompute;


	//testing
	VkImage m_textureImage;
	VkDeviceMemory m_textureImageMemory;
	VkImageView m_textureImageView;
	VkSampler m_textureSampler;
	void createTextureRessources();

	std::vector<Vertex2D> m_vertices2D = {
	{{-1.0f, -1.0f},	{1.0f, 0.0f, 0.0f}},	//ROT
	{{1.0f, -1.0f},		{0.0f, 1.0f, 0.0f}},	//GRÜN
	{{1.0f, 1.0f},		{0.0f, 0.0f, 1.0f}},	//BLAU
	{{-1.0f, 1.0f},		{1.0f, 1.0f, 1.0f}},	//WEIß
	};

#pragma endregion


	UserInput m_input;

	double m_xNew = 0, m_yNew = 0;
	double m_xPos = 0, m_yPos = 0;

	float m_runTime = 0.0f;
	float m_lastTime = 0.0f;
	float m_deltaTime = 0.0f;

	float m_speed = 1;
	float m_mouseSpeed = 0.0005f;

	void getTime();
	void updateCamera();
	void initScene();

	virtual void InitSceneObjects(void) = 0;

	std::vector<Scene> m_scenes;
	int m_currentScene = 0;
	Camera* m_pCamera = nullptr;
};

#endif // !HELLO_TRIANGLE_APPLICATION_H



