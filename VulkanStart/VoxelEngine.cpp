#include "VoxelEngine.h"

void VoxelEngine::run()
{
	initWindow();
	initScene();
	if (m_useCompute) { initVulkanCompute(); }
	else { initVulkan(); }
	mainLoop();
	cleanup();
}

static void framebufferResiceCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<VoxelEngine*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}

void VoxelEngine::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Voxel Engine", nullptr, nullptr);
	glfwSetWindowUserPointer(m_pWindow, this); 
	glfwSetFramebufferSizeCallback(m_pWindow, framebufferResiceCallback);

	glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported()) 
	{
		glfwSetInputMode(m_pWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	
}

void VoxelEngine::initVulkan()
{
	createAndExecuteShaderBat();
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();

	createRenderPass();
	createDescriptorLayout();
	createGraphicsPipeline();

	createCommandPool();
	createDepthResources();
	createFramebuffers();
	createVertexBuffer();
	createIndexBuffer(); 
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	createSyncObjects();  
}

void VoxelEngine::initVulkanCompute()
{
	createAndExecuteShaderBat();	//same
	createInstance();				//same
	setupDebugMessenger();			//same
	createSurface();				//same
	pickPhysicalDevice();			//same
	createLogicalDevice();			//same
	createSwapChain();				//same
	createImageViews();				//same

	createRenderPassCompute();
	createDescriptorLayoutCompute();
	createGraphicsAndComputePipeline();
	createFramebuffersCompute();	

	createCommandPool();			//same

	createShaderStorageBuffersCompute();

	createTextureRessources();

	createDescriptorPoolCompute();			
	createDescriptorSetsCompute();

	createCommandBuffers();			//same

	createCommandBuffersCompute();

	createSyncObjects();			//same
}


void VoxelEngine::mainLoop()
{
	if (!m_useCompute) 
	{
		while (!glfwWindowShouldClose(m_pWindow))
		{
			getTime();
			processInput();
			updateCamera();
			glfwPollEvents();
			drawFrame();
		}
	}
	else {
		while (!glfwWindowShouldClose(m_pWindow))
		{
			getTime();
			processInput();
			updateCamera();
			glfwPollEvents();
			drawFrameCompute();
		}
	}

	vkDeviceWaitIdle(m_logicalDevice);
}

void VoxelEngine::cleanup()
{
	if (m_pCamera) {
		m_pCamera = nullptr;
	}
	
	cleanupSwapchain();

	vkDestroyPipeline(m_logicalDevice, m_pipelineCompute, nullptr);
	vkDestroyPipeline(m_logicalDevice, m_graphicsPipeline, nullptr); 

	vkDestroyPipelineLayout(m_logicalDevice, m_pipelineLayout, nullptr); 
	vkDestroyPipelineLayout(m_logicalDevice, m_pipelineLayoutCompute, nullptr);

	vkDestroyRenderPass(m_logicalDevice, m_renderPass, nullptr); 

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(m_logicalDevice, m_uniformBuffers[i], nullptr);
		vkFreeMemory(m_logicalDevice, m_uniformBuffersMemory[i], nullptr);
	}

	vkDestroyDescriptorPool(m_logicalDevice, m_descriptorPool, nullptr); 

	vkDestroyDescriptorSetLayout(m_logicalDevice, m_descriptorSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(m_logicalDevice, m_descriptorSetLayoutCompute, nullptr);

	vkDestroyBuffer(m_logicalDevice, m_indexBuffer, nullptr);
	vkFreeMemory(m_logicalDevice, m_indexBufferMemory, nullptr);

	vkDestroyBuffer(m_logicalDevice, m_vertexBuffer, nullptr);
	vkFreeMemory(m_logicalDevice, m_vertexBufferMemory, nullptr);

	vkDestroyBuffer(m_logicalDevice, m_voxelBuffer, nullptr);
	vkFreeMemory(m_logicalDevice, m_voxelBufferMemory, nullptr);

	vkDestroyImage(m_logicalDevice, m_textureImage, nullptr);
	vkFreeMemory(m_logicalDevice, m_textureImageMemory, nullptr);
	vkDestroyImageView(m_logicalDevice, m_textureImageView, nullptr);
	vkDestroySampler(m_logicalDevice, m_textureSampler, nullptr);

	
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(m_logicalDevice, m_renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_logicalDevice, m_imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(m_logicalDevice, m_inFlightFences[i], nullptr);

		vkDestroySemaphore(m_logicalDevice, m_computeFinishedSemaphores[i], nullptr);
		vkDestroyFence(m_logicalDevice, m_computeInFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);

	vkDestroyDevice(m_logicalDevice, nullptr);

	if (enableValidationLayers) { 
		DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);

	glfwDestroyWindow(m_pWindow);

	glfwTerminate();
}

void VoxelEngine::processInput()
{
	//Mouse Input Enable and Disable
	if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		if (glfwRawMouseMotionSupported())
		{
			glfwSetInputMode(m_pWindow, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}
	}
	if (glfwGetMouseButton(m_pWindow, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		if (glfwRawMouseMotionSupported())
		{
			glfwSetInputMode(m_pWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
	}

	//Update Makros
	if (!m_useCompute) 
	{
		if (glfwGetKey(m_pWindow, GLFW_KEY_U) == GLFW_PRESS) {
			updateBuffers();
		}
	}

	//Mouse Input for Camera Movement
	if (!m_useCompute) 
	{
		glfwGetCursorPos(m_pWindow, &m_xNew, &m_yNew);
		m_input.GetMouseInputDelta(m_xNew, m_yNew, &m_xPos, &m_yPos);

		//Keyboard Input for Camera Movement
		m_input.SetNull();

		if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS) {
			m_input.Forward();
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS) {
			m_input.Backward();
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS) {
			m_input.Left();
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS) {
			m_input.Right();
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
			m_input.Up();
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			m_input.Down();
		}
	}
	
	else //input is reversed
	{
		glfwGetCursorPos(m_pWindow, &m_xNew, &m_yNew);
		m_input.GetMouseInputDelta(-m_xNew, m_yNew, &m_xPos, &m_yPos);

		//Keyboard Input for Camera Movement
		m_input.SetNull();

		if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS) {
			m_input.Forward();
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS) {
			m_input.Backward();
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS) {
			m_input.Left();
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS) {
			m_input.Right();
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
			m_input.Up();
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			m_input.Down();
		}
	}


	
}

void VoxelEngine::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) 
	{
		throw std::runtime_error("Fail: validation layers requested, but not available!");
	}
	else {
		std::cout << "Success: found requested validation layers!" << std::endl;
	}

	VkApplicationInfo appInfo{}; 
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; 
	appInfo.pApplicationName = "Hello Triangle"; 
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); 
	appInfo.pEngineName = "No Engine"; 
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); 
	appInfo.apiVersion = VK_API_VERSION_1_0;


	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) 
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else 
	{
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}


	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance); 

	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vkinstance!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created vkinstance!" << std::endl;
		std::cout << "Instance Create Info Type: " << createInfo.sType << std::endl;
		std::cout << "Instance Application Name: " << appInfo.pApplicationName << std::endl;
		std::cout << "Instance Type: " << appInfo.sType << std::endl;
		std::cout << "Instance Engine Name: " << appInfo.pEngineName << std::endl;   
	}

}

bool VoxelEngine::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

void VoxelEngine::createAndExecuteShaderBat()
{
	std::ofstream batch; 
	batch.open("shaderbatchfile.bat", std::ios::out);

	batch << "glslc.exe shaders/shader.vert -o shaders/vert.spv\n";
	batch << "glslc.exe shaders/shader.frag -o shaders/frag.spv\n";

	batch << "glslc.exe shaders/shader.comp -o shaders/comp.spv\n";
	batch << "glslc.exe shaders/compshader.vert -o shaders/compvert.spv\n";
	batch << "glslc.exe shaders/compshader.frag -o shaders/compfrag.spv\n";

	batch.close();
	
	system("shaderbatchfile.bat");
	std::filesystem::remove("shaderbatchfile.bat"); 
}

std::vector<const char*> VoxelEngine::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VoxelEngine::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

void VoxelEngine::setupDebugMessenger()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) { 
		throw std::runtime_error("failed to set up debug messenger!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: debug messenger was set up!" << std::endl;
	}
}

void VoxelEngine::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

VkResult VoxelEngine::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VoxelEngine::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

//scoring system to check which GraphicsCard is most suitable
int VoxelEngine::rateDeviceSuitability(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	int score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!deviceFeatures.geometryShader) {
		return 0;
	}

	return score;
}

//check if any advice is suitable 
bool VoxelEngine::isDeviceSuitable(VkPhysicalDevice device)
{
	//return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;

	QueueFamilyIndices indices = findQueueFamilies(device, true);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	//needs to have at least one supported image format + one supported presentation mode
	bool swapChainAdequate = false;
	if (extensionsSupported) 
	{ 
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device); 
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty(); 
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool VoxelEngine::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount); 
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end()); 

	for (const auto& extension : availableExtensions) { 
		requiredExtensions.erase(extension.extensionName); 
	}

	return requiredExtensions.empty();
}

void VoxelEngine::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice, true);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos; 
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsAndComputeFamily.value(), indices.presentFamily.value() }; 

	std::cout << "The " << indices.graphicsAndComputeFamily.value() << "th QueueFamily supports every required QueueFlag for graphical needs!" << std::endl;
	std::cout << "The " << indices.presentFamily.value() << "th QueueFamily supports presentation to a surface!" << std::endl;


	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) { 
		VkDeviceQueueCreateInfo queueCreateInfo{}; 
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; 
		queueCreateInfo.queueFamilyIndex = queueFamily; 
		queueCreateInfo.queueCount = 1; 
		queueCreateInfo.pQueuePriorities = &queuePriority; 
		queueCreateInfos.push_back(queueCreateInfo); 
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data(); 

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_logicalDevice) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create logical device!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created logical device!" << std::endl;
		std::cout << "Enabled Extension Count:  " << createInfo.enabledExtensionCount << std::endl;
		std::cout << "Extension Names: " << *createInfo.ppEnabledExtensionNames << std::endl;
		std::cout << "Enabled Validation Layer Count: " << createInfo.enabledLayerCount << std::endl; 
		if (createInfo.enabledLayerCount > 0) {
			std::cout << "Layer Names: " << *createInfo.ppEnabledLayerNames << std::endl;
		}
		std::cout << "Different QueueFamilies Count: " << createInfo.queueCreateInfoCount << std::endl;

	}

	vkGetDeviceQueue(m_logicalDevice, indices.graphicsAndComputeFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_logicalDevice, indices.graphicsAndComputeFamily.value(), 0, &m_queueCompute);
	vkGetDeviceQueue(m_logicalDevice, indices.presentFamily.value(), 0, &m_presentQueue);
}

void VoxelEngine::createSurface()
{
	/*VkWin32SurfaceCreateInfoKHR createInfo{}; 
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = glfwGetWin32Window(pWindow);
	createInfo.hinstance = GetModuleHandle(nullptr);
	
	if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) 
	{
		throw std::runtime_error("window surface creation has failed!");
	}*/

	if (glfwCreateWindowSurface(m_instance, m_pWindow, nullptr, &m_surface) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create window surface!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created window surface!" << std::endl;
	}
}

SwapChainSupportDetails VoxelEngine::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

	uint32_t formatCount; 
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr); 

	if (formatCount != 0) { 
		details.formats.resize(formatCount); 
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data()); 
	}

	uint32_t presentModeCount; 
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr); 

	if (presentModeCount != 0) { 
		details.presentModes.resize(presentModeCount); 
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data()); 
	}

	return details;
}

VkSurfaceFormatKHR VoxelEngine::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) 
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR VoxelEngine::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VoxelEngine::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
#undef max
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(m_pWindow, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void VoxelEngine::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice); 

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats); 
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes); 
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities); 

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1; 

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
	{ 
		imageCount = swapChainSupport.capabilities.maxImageCount; 
	}

	VkSwapchainCreateInfoKHR createInfo{}; 
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR; 
	createInfo.surface = m_surface; 

	createInfo.minImageCount = imageCount; 
	createInfo.imageFormat = surfaceFormat.format; 
	createInfo.imageColorSpace = surfaceFormat.colorSpace; 
	createInfo.imageExtent = extent; 
	createInfo.imageArrayLayers = 1; 
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_STORAGE_BIT;

	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice, false); 
	uint32_t queueFamilyIndices[] = { indices.graphicsAndComputeFamily.value(), indices.presentFamily.value() }; 

	if (indices.graphicsAndComputeFamily != indices.presentFamily) { 
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; 
		createInfo.queueFamilyIndexCount = 2; 
		createInfo.pQueueFamilyIndices = queueFamilyIndices; 
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; 
		createInfo.queueFamilyIndexCount = 0; // Optional 
		createInfo.pQueueFamilyIndices = nullptr; // Optional 
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform; 
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_logicalDevice, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create swap chain!");
	}
	else {
		std::cout << "" << std::endl; 
		std::cout << "Success: created swap chain!" << std::endl; 
	}

	vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &imageCount, nullptr);
	m_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &imageCount, m_swapChainImages.data());

	m_swapChainImageFormat = surfaceFormat.format; 
	m_swapChainExtent = extent; 
}

void VoxelEngine::createImageViews()
{
	m_swapChainImageViews.resize(m_swapChainImages.size()); 

	for (size_t i = 0; i < m_swapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};

		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; 
		createInfo.image = m_swapChainImages[i]; 

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_swapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; 
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY; 
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY; 
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY; 

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; 
		createInfo.subresourceRange.baseMipLevel = 0; 
		createInfo.subresourceRange.levelCount = 1; 
		createInfo.subresourceRange.baseArrayLayer = 0; 
		createInfo.subresourceRange.layerCount = 1; 

		if (vkCreateImageView(m_logicalDevice, &createInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
		else {
			std::cout << "" << std::endl;
			std::cout << "Success: created image view number: " << i << "!" << std::endl;
		}

	}

}

static std::vector<char> readFile(const std::string &filename) 
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

void VoxelEngine::createGraphicsPipeline()
{
	auto vertShaderCode = readFile("shaders/vert.spv");
	auto fragShaderCode = readFile("shaders/frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo }; 


	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; 
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); 


	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;


	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;	//Insert VK_POLYGON_MODE_LINE to create a wireframe view (needs an additional GPU feature)
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;	//Which faces should be culled
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //How are the front faces determined 
	rasterizer.depthBiasEnable = VK_FALSE;


	VkPipelineMultisampleStateCreateInfo multisampling{};  
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO; 
	multisampling.sampleShadingEnable = VK_FALSE; 
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; 


	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;  
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;  
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;  
	depthStencil.depthBoundsTestEnable = VK_FALSE;  
	depthStencil.minDepthBounds = 0.0f; // Optional 
	depthStencil.maxDepthBounds = 1.0f; // Optional 
	depthStencil.stencilTestEnable = VK_FALSE; 
	depthStencil.front = {}; // Optional 
	depthStencil.back = {}; // Optional 


	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;


	VkPipelineColorBlendStateCreateInfo colorBlending{}; 
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO; 
	colorBlending.logicOpEnable = VK_FALSE; 
	colorBlending.logicOp = VK_LOGIC_OP_COPY; 
	colorBlending.attachmentCount = 1; 
	colorBlending.pAttachments = &colorBlendAttachment; 
	colorBlending.blendConstants[0] = 0.0f; 
	colorBlending.blendConstants[1] = 0.0f; 
	colorBlending.blendConstants[2] = 0.0f; 
	colorBlending.blendConstants[3] = 0.0f;


	std::vector<VkDynamicState> dynamicStates = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();


	VkPipelineLayoutCreateInfo pipelineLayoutInfo{}; 
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1; 
	pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;

	if (vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created graphics pipeline layout " << std::endl;
	}


	VkGraphicsPipelineCreateInfo pipelineInfo{}; 
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO; 
	pipelineInfo.stageCount = 2; 
	pipelineInfo.pStages = shaderStages; 

	pipelineInfo.pVertexInputState = &vertexInputInfo; 
	pipelineInfo.pInputAssemblyState = &inputAssembly; 
	pipelineInfo.pViewportState = &viewportState; 
	pipelineInfo.pRasterizationState = &rasterizer; 
	pipelineInfo.pMultisampleState = &multisampling; 
	pipelineInfo.pDepthStencilState = &depthStencil;  
	pipelineInfo.pColorBlendState = &colorBlending; 
	pipelineInfo.pDynamicState = &dynamicState; 

	pipelineInfo.layout = m_pipelineLayout; 

	pipelineInfo.renderPass = m_renderPass; 
	pipelineInfo.subpass = 0; 
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; 

	if (vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created graphics pipeline " << std::endl;
	}



	vkDestroyShaderModule(m_logicalDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_logicalDevice, vertShaderModule, nullptr);
}

void VoxelEngine::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; 
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; 
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; 
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; 
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; 

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; 
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; 

	VkAttachmentReference colorAttachmentRef{}; 
	colorAttachmentRef.attachment = 0; 
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1; 
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;


	VkSubpassDependency dependency{}; 
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment }; 
	VkRenderPassCreateInfo renderPassInfo{}; 
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO; 
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size()); 
	renderPassInfo.pAttachments = attachments.data(); 
	renderPassInfo.subpassCount = 1; 
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1; 
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(m_logicalDevice, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created render pass " << std::endl;
	}
}

void VoxelEngine::createDescriptorLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional 

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(m_logicalDevice, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created descriptor set layout" << std::endl;
	}

}

VkShaderModule VoxelEngine::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;

	if (vkCreateShaderModule(m_logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create shader module!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created shader module" << std::endl;
	}

	return shaderModule;
}

void VoxelEngine::createFramebuffers()
{
	m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

	for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
		std::array<VkImageView, 2> attachments = {
			m_swapChainImageViews[i],
			m_depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_swapChainExtent.width;
		framebufferInfo.height = m_swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_logicalDevice, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer number: " + i);
		}
		else {
			std::cout << "" << std::endl;
			std::cout << "Success: created frame buffer number: " << i << std::endl;
		}
	}
}

void VoxelEngine::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice, false);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();

	if (vkCreateCommandPool(m_logicalDevice, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created command pool" << std::endl;
	}
}

void VoxelEngine::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();
	VkBuffer stagingBuffer; 
	VkDeviceMemory stagingBufferMemory; 
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory); 

	void* data;
	vkMapMemory(m_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_logicalDevice, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

	copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

	vkDestroyBuffer(m_logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(m_logicalDevice, stagingBufferMemory, nullptr);
}

uint32_t VoxelEngine::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{ 
	VkPhysicalDeviceMemoryProperties memProperties; 
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties); 

	std::cout << "" << std::endl;
	std::cout << "Memory Type Count: " << memProperties.memoryTypeCount << std::endl;
	std::cout << "Memory Heap Count: " << memProperties.memoryHeapCount << std::endl;

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) { 
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i; 
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void VoxelEngine::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_logicalDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: allocated vertex buffer memory" << std::endl;
	}

	vkBindBufferMemory(m_logicalDevice, buffer, bufferMemory, 0);
}

void VoxelEngine::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer); 

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_graphicsQueue);

	vkFreeCommandBuffers(m_logicalDevice, m_commandPool, 1, &commandBuffer);
}

void VoxelEngine::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_indices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_logicalDevice, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);

	copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

	vkDestroyBuffer(m_logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(m_logicalDevice, stagingBufferMemory, nullptr);
}

void VoxelEngine::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	m_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			m_uniformBuffers[i], m_uniformBuffersMemory[i]);

		vkMapMemory(m_logicalDevice, m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
	}
}

void VoxelEngine::createDescriptorPool() 
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(m_logicalDevice, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void VoxelEngine::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT); 

	if (vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: allocated descriptor sets" << std::endl;
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(m_logicalDevice, 1, &descriptorWrite, 0, nullptr);
	}
}

void VoxelEngine::createCommandBuffers()
{
	m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);   

	VkCommandBufferAllocateInfo allocInfo{}; 
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; 
	allocInfo.commandPool = m_commandPool; 
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; 
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size(); 

	if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created command buffer" << std::endl;
	}
}

void VoxelEngine::createSyncObjects()
{
	m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_computeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

	m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	m_computeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_logicalDevice, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics synchronization objects for a frame!");
		}
		if (vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_computeFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_logicalDevice, &fenceInfo, nullptr, &m_computeInFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute synchronization objects for a frame!");
		}
	}

}

void VoxelEngine::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, std::vector<VkDescriptorSet> a_descriptorSets)
{
	VkCommandBufferBeginInfo beginInfo{}; 
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO; 
	beginInfo.flags = 0; // Optional 
	beginInfo.pInheritanceInfo = nullptr; // Optional 

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) { 
		throw std::runtime_error("failed to begin recording command buffer!");
	}
	 
	VkRenderPassBeginInfo renderPassInfo{}; 
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO; 
	renderPassInfo.renderPass = m_renderPass; 
	renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 }; 
	renderPassInfo.renderArea.extent = m_swapChainExtent; 

	std::array<VkClearValue, 2> clearValues{};  
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} }; 
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size()); 
	renderPassInfo.pClearValues = clearValues.data();  

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); 
		
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline); 

	VkViewport viewport{}; 
	viewport.x = 0.0f; 
	viewport.y = 0.0f; 
	viewport.width = static_cast<float>(m_swapChainExtent.width); 
	viewport.height = static_cast<float>(m_swapChainExtent.height); 
	viewport.minDepth = 0.0f; 
	viewport.maxDepth = 1.0f; 
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport); 

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


	VkBuffer vertexBuffers[] = { m_vertexBuffer };
	VkDeviceSize offsets[] = { 0 }; 
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets); 

	vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);


	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &a_descriptorSets[m_currentFrame], 0, nullptr);

	//vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_vertices2D.size()), 1, 0, 0);
		
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0); //DRAW COMMAND


	vkCmdEndRenderPass(commandBuffer); 

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) { 
		throw std::runtime_error("failed to record command buffer!"); 
	} 
}

void VoxelEngine::drawFrame()
{
	vkWaitForFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex; 
	VkResult result = vkAcquireNextImageKHR(m_logicalDevice, m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	updateUniformBuffer(m_currentFrame);

	vkResetFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrame]);

	vkResetCommandBuffer(m_commandBuffers[m_currentFrame], 0);
	recordCommandBuffer(m_commandBuffers[m_currentFrame], imageIndex, m_descriptorSets);

	VkSubmitInfo submitInfo{}; 
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO; 

	VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };								//what to wait for 
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };		//when to wait
	submitInfo.waitSemaphoreCount = 1; 
	submitInfo.pWaitSemaphores = waitSemaphores;												//tell submitInfo what to wait for
	submitInfo.pWaitDstStageMask = waitStages; 
	submitInfo.commandBufferCount = 1; 
	submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];

	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };								//which semaphore to signal when finished rendering

	submitInfo.signalSemaphoreCount = 1; 
	submitInfo.pSignalSemaphores = signalSemaphores;											//tell submitInfo who to tell when finished

	if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!"); 
	} 


	VkPresentInfoKHR presentInfo{}; 
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR; 
	presentInfo.waitSemaphoreCount = 1; 
	presentInfo.pWaitSemaphores = signalSemaphores; 


	VkSwapchainKHR swapChains[] = { m_swapChain }; 
	presentInfo.swapchainCount = 1; 
	presentInfo.pSwapchains = swapChains; 
	presentInfo.pImageIndices = &imageIndex; 
	presentInfo.pResults = nullptr; // Optional 

	result = vkQueuePresentKHR(m_presentQueue, &presentInfo); 

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VoxelEngine::updateUniformBuffer(uint32_t currentImage)
{
	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), m_runTime * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(m_pCamera->GetPosition3(), m_pCamera->GetForward3(), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), m_swapChainExtent.width / (float)m_swapChainExtent.height, 0.1f, 1000.0f); 
	ubo.proj[1][1] *= -1; 


	ubo.camPosition = m_pCamera->GetPosition3();
	ubo.camForward = m_pCamera->GetForward3();
	ubo.camUp = m_pCamera->GetUp3();
	ubo.camRight = m_pCamera->GetRight3();

	memcpy(m_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo)); 
}

void VoxelEngine::recreateSwapchain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(m_pWindow, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(m_pWindow, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_logicalDevice);

	cleanupSwapchain();

	createSwapChain();
	createImageViews();
	if (!m_useCompute) 
	{
		createDepthResources();
		createFramebuffers();
	}
	else 
	{
		createFramebuffersCompute();
	}
	
}

void VoxelEngine::cleanupSwapchain()
{
	vkDestroyImageView(m_logicalDevice, m_depthImageView, nullptr); 
	vkDestroyImage(m_logicalDevice, m_depthImage, nullptr); 
	vkFreeMemory(m_logicalDevice, m_depthImageMemory, nullptr); 

	for (auto framebuffer : m_swapChainFramebuffers) {
		vkDestroyFramebuffer(m_logicalDevice, framebuffer, nullptr);
	}

	for (auto imageView : m_swapChainImageViews) {
		vkDestroyImageView(m_logicalDevice, imageView, nullptr);
	}

	vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
}

void VoxelEngine::createDepthResources()
{ 
	VkFormat depthFormat = findDepthFormat(); 

	createImage(m_swapChainExtent.width, m_swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, 
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImage, m_depthImageMemory, VK_IMAGE_LAYOUT_UNDEFINED);
	m_depthImageView = createImageView(m_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT); 

	transitionImageLayout(m_depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

VkFormat VoxelEngine::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

VkFormat VoxelEngine::findDepthFormat()
{
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, 
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);

}

bool VoxelEngine::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VoxelEngine::createImage(uint32_t a_width, uint32_t a_height, VkFormat a_format, VkImageTiling a_tiling, VkImageUsageFlags a_usage, VkMemoryPropertyFlags a_properties, VkImage& a_image, VkDeviceMemory& a_imageMemory, VkImageLayout a_initialLayout)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = a_width;
	imageInfo.extent.height = a_height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;

	imageInfo.format = a_format;
	imageInfo.tiling = a_tiling;

	imageInfo.initialLayout = a_initialLayout;	
	imageInfo.usage = a_usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;


	if (vkCreateImage(m_logicalDevice, &imageInfo, nullptr, &a_image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_logicalDevice, a_image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, a_properties);

	if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &a_imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(m_logicalDevice, a_image, a_imageMemory, 0);
}

VkImageView VoxelEngine::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(m_logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView; 
}

void VoxelEngine::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT; 

		sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (hasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer VoxelEngine::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void VoxelEngine::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_graphicsQueue);

	vkFreeCommandBuffers(m_logicalDevice, m_commandPool, 1, &commandBuffer);
}

void VoxelEngine::updateBuffers()
{
	vkDeviceWaitIdle(m_logicalDevice);

	vkDestroyBuffer(m_logicalDevice, m_indexBuffer, nullptr);
	vkFreeMemory(m_logicalDevice, m_indexBufferMemory, nullptr);

	vkDestroyBuffer(m_logicalDevice, m_vertexBuffer, nullptr);
	vkFreeMemory(m_logicalDevice, m_vertexBufferMemory, nullptr);

	//m_scenes.at(m_currentScene).OverwriteVertsAndIndices(m_vertices, m_indices);
	m_scenes.at(m_currentScene).OverwriteVertsAndIndicesMT(m_vertices, m_indices); 

	createVertexBuffer(); 
	createIndexBuffer();
}

void VoxelEngine::getTime()
{
	auto currentTime = std::chrono::high_resolution_clock::now(); 

	m_runTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count(); 

	m_deltaTime = m_runTime - m_lastTime; 

	m_lastTime = m_runTime;
}

void VoxelEngine::updateCamera()
{
	m_pCamera->Rotate(m_input, m_mouseSpeed);
	m_pCamera->Move(m_input, m_deltaTime, m_speed);
}

void VoxelEngine::initScene()
{
	Camera cam = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	m_scenes.push_back(Scene(cam));
	m_currentScene = 0;
	m_pCamera = &m_scenes.at(m_currentScene).GetCamera();

	InitSceneObjects();

	std::cout << "" << std::endl;
	std::cout << "Success: initialized Scene" << std::endl;
}

void VoxelEngine::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	if (deviceCount == 0)  
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	else{
		std::cout << "" << std::endl;
		std::cout << "Success: found GPUs with vulkan support!" << std::endl;
		std::vector<VkPhysicalDevice> devices(deviceCount);  
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data()); 

		for (auto& device : devices) {
			VkPhysicalDeviceProperties deviceProperties; 
			vkGetPhysicalDeviceProperties(device, &deviceProperties);

			std::cout << "Device Name: " << deviceProperties.deviceName <<std::endl;
		}
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);  
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());  

	//std::multimap<int, VkPhysicalDevice> candidates;

	//for (const VkPhysicalDevice device : devices) {
	//	int score = rateDeviceSuitability(device);
	//	candidates.insert(std::make_pair(score, device));
	//	std::cout << "Device: " << device << " has a score of: " << score << std::endl;
	//}

	//// Check if the best candidate is suitable at all
	//if (candidates.rbegin()->first > 0) {
	//	physicalDevice = candidates.rbegin()->second;
	//}
	//else {
	//	throw std::runtime_error("failed to find a suitable GPU!");
	//}

	for (const auto& device : devices) 
	{
		if (isDeviceSuitable(device)) 
		{
			m_physicalDevice = device;
			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE) 
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
	else {
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);
		std::cout << "" << std::endl;
		std::cout << "Success: found suitable GPU! : " << properties.deviceName << std::endl;
	}


}

QueueFamilyIndices VoxelEngine::findQueueFamilies(VkPhysicalDevice device, bool debugQueueFamilies)
{
	QueueFamilyIndices indices; 

	uint32_t queueFamilyCount; 
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	if (debugQueueFamilies) 
	{
		for (auto& queueFamily : queueFamilies) {
			std::cout << "Queue Count in Family: " << queueFamily.queueCount << std::endl;
			std::cout << "Queue Flags:" << std::endl;

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				std::cout << "| VK_QUEUE_GRAPHICS_BIT " << "-> Graphics Operations" << std::endl;
			}
			if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				std::cout << "| VK_QUEUE_COMPUTE_BIT " << "-> Compute Operations" << std::endl;
			}
			if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				std::cout << "| VK_QUEUE_TRANSFER_BIT " << "-> Transfer Operations" << std::endl;
			}
			if (queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
			{
				std::cout << "| VK_QUEUE_SPARSE_BINDING_BIT " << "-> Sparse Memory Management" << std::endl;
			}
			if (queueFamily.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR)
			{
				std::cout << "| VK_QUEUE_VIDEO_DECODE_BIT_KHR " << "-> Video Decode Operations" << std::endl;
			}
			if (queueFamily.queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR)
			{
				std::cout << "| VK_QUEUE_VIDEO_ENCODE_BIT_KHR " << "-> Video Encode Operations" << std::endl;
			}
			if (queueFamily.queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV)
			{
				std::cout << "| VK_QUEUE_OPTICAL_FLOW_BIT_NV " << "-> Optical Flow Operations" << std::endl;
			}
			if (queueFamily.queueFlags & VK_QUEUE_PROTECTED_BIT)
			{
				std::cout << "| VK_QUEUE_PROTECTED_BIT " << "-> VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT ???" << std::endl;
			}
		}
	}
	

	uint32_t i = 0; 
	for (const auto& queueFamily : queueFamilies) {
		if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
			indices.graphicsAndComputeFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

#pragma region ComputeRaycasting

void VoxelEngine::createRenderPassCompute()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(m_logicalDevice, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}

}

void VoxelEngine::createDescriptorLayoutCompute()
{
	std::array<VkDescriptorSetLayoutBinding, 3> layoutBindings{};
	//Camera UBO
	layoutBindings[0].binding = 0;
	layoutBindings[0].descriptorCount = 1;
	layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings[0].pImmutableSamplers = nullptr;
	layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	//Voxel SSBO
	layoutBindings[1].binding = 1;
	layoutBindings[1].descriptorCount = 1;
	layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	layoutBindings[1].pImmutableSamplers = nullptr;
	layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	//Image Buffer
	layoutBindings[2].binding = 2;
	layoutBindings[2].descriptorCount = 1;
	layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	layoutBindings[2].pImmutableSamplers = nullptr;
	layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;


	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = layoutBindings.size();
	layoutInfo.pBindings = layoutBindings.data();

	if (vkCreateDescriptorSetLayout(m_logicalDevice, &layoutInfo, nullptr, &m_descriptorSetLayoutCompute) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute descriptor set layout!");
	}

}

void VoxelEngine::createGraphicsAndComputePipeline()
{
#pragma region Compute
	auto computeShaderCode = readFile("shaders/comp.spv");

	VkShaderModule computeShaderModule = createShaderModule(computeShaderCode);

	VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
	computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	computeShaderStageInfo.module = computeShaderModule;
	computeShaderStageInfo.pName = "main";

	VkPipelineLayoutCreateInfo computePipelineLayoutInfo{};
	computePipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	computePipelineLayoutInfo.setLayoutCount = 1;
	computePipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayoutCompute;

	if (vkCreatePipelineLayout(m_logicalDevice, &computePipelineLayoutInfo, nullptr, &m_pipelineLayoutCompute) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute pipeline layout!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created compute pipeline layout " << std::endl;
	}

	VkComputePipelineCreateInfo computePipelineInfo{};
	computePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineInfo.layout = m_pipelineLayoutCompute;
	computePipelineInfo.stage = computeShaderStageInfo;

	if (vkCreateComputePipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &computePipelineInfo, nullptr, &m_pipelineCompute) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute pipeline!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created compute pipeline" << std::endl;
	}


	vkDestroyShaderModule(m_logicalDevice, computeShaderModule, nullptr);
#pragma endregion

#pragma region Graphics

	auto vertShaderCode = readFile("shaders/compvert.spv");
	auto fragShaderCode = readFile("shaders/compfrag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };


	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = Vertex2D::getBindingDescription();
	auto attributeDescriptions = Vertex2D::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();


	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;


	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;	//Insert VK_POLYGON_MODE_LINE to create a wireframe view (needs an additional GPU feature)
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;	//Which faces should be culled
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; //How are the front faces determined VK_FRONT_FACE_COUNTER_CLOCKWISE
	rasterizer.depthBiasEnable = VK_FALSE;


	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;


	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;


	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;


	std::vector<VkDynamicState> dynamicStates = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();


	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	computePipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	computePipelineLayoutInfo.setLayoutCount = 1;
	computePipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayoutCompute;

	if (vkCreatePipelineLayout(m_logicalDevice, &computePipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created pipeline layout " << std::endl;
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};

	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;

	pipelineInfo.layout = m_pipelineLayout;

	pipelineInfo.renderPass = m_renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created graphics pipeline " << std::endl;
	}

	vkDestroyShaderModule(m_logicalDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_logicalDevice, vertShaderModule, nullptr);

#pragma endregion

}


void VoxelEngine::createFramebuffersCompute()
{
	m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

	for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			m_swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_swapChainExtent.width;
		framebufferInfo.height = m_swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_logicalDevice, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void VoxelEngine::createShaderStorageBuffersCompute()
{
	//Vertex Buffer
	VkDeviceSize vertexBufferSize = sizeof(m_vertices2D[0]) * m_vertices2D.size();;
	VkBuffer vertexStagingBuffer;
	VkDeviceMemory vertexStagingBufferMemory;
	createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexStagingBuffer, vertexStagingBufferMemory);

	void* vertexData;
	vkMapMemory(m_logicalDevice, vertexStagingBufferMemory, 0, vertexBufferSize, 0, &vertexData);
	memcpy(vertexData, m_vertices2D.data(), (size_t)vertexBufferSize);
	vkUnmapMemory(m_logicalDevice, vertexStagingBufferMemory);

	createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

	copyBuffer(vertexStagingBuffer, m_vertexBuffer, vertexBufferSize);

	vkDestroyBuffer(m_logicalDevice, vertexStagingBuffer, nullptr);
	vkFreeMemory(m_logicalDevice, vertexStagingBufferMemory, nullptr);

	//Index Buffer
	createIndexBuffer();

	//Uniform Buffer
	createUniformBuffers();

	//Voxel SSBO
	//Get Voxel Information

	std::vector<Voxel> voxel = m_scenes[m_currentScene].GetVoxel();

	VkDeviceSize voxelBufferSize = sizeof(Voxel) * m_scenes[m_currentScene].GetVoxel().size();

	//Create a staging buffer used to upload data to the gpu
	VkBuffer voxelStagingBuffer;
	VkDeviceMemory voxelStagingBufferMemory;
	createBuffer(voxelBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, voxelStagingBuffer, voxelStagingBufferMemory);

	void* voxelData;
	vkMapMemory(m_logicalDevice, voxelStagingBufferMemory, 0, voxelBufferSize, 0, &voxelData);
	memcpy(voxelData, voxel.data(), (size_t)voxelBufferSize);
	vkUnmapMemory(m_logicalDevice, voxelStagingBufferMemory);
	
	createBuffer(voxelBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT  | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_voxelBuffer, m_voxelBufferMemory);
	copyBuffer(voxelStagingBuffer, m_voxelBuffer, voxelBufferSize);

	//Destroy staging Buffer
	vkDestroyBuffer(m_logicalDevice, voxelStagingBuffer, nullptr);
	vkFreeMemory(m_logicalDevice, voxelStagingBufferMemory, nullptr);

	//Image 
	createImage(WIDTH, HEIGHT, VK_FORMAT_R8G8B8A8_SNORM, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_STORAGE_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_textureImage, m_textureImageMemory, VK_IMAGE_LAYOUT_UNDEFINED);

	transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

}

void VoxelEngine::createDescriptorPoolCompute() 
{
	std::array<VkDescriptorPoolSize, 3> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE; //VK_DESCRIPTOR_TYPE_STORAGE_IMAGE VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(m_logicalDevice, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created descriptor pool" << std::endl;
	}
}

void VoxelEngine::createDescriptorSetsCompute()
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayoutCompute);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	m_descriptorSetsCompute.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, m_descriptorSetsCompute.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

		VkDescriptorBufferInfo uniformBufferInfo{};
		uniformBufferInfo.buffer = m_uniformBuffers[i];
		uniformBufferInfo.offset = 0;
		uniformBufferInfo.range = sizeof(UniformBufferObject);

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_descriptorSetsCompute[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &uniformBufferInfo;

		VkDescriptorBufferInfo voxelStorageBufferInfo{};
		voxelStorageBufferInfo.buffer = m_voxelBuffer;
		voxelStorageBufferInfo.offset = 0;
		voxelStorageBufferInfo.range = sizeof(Voxel) * m_scenes[m_currentScene].GetVoxel().size();

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_descriptorSetsCompute[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &voxelStorageBufferInfo;

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo.imageView = m_textureImageView;
		imageInfo.sampler = m_textureSampler;

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstSet = m_descriptorSetsCompute[i];
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE; 
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(m_logicalDevice, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
	}
}

void VoxelEngine::createCommandBuffersCompute()
{
	m_commandBuffersCompute.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffersCompute.size();

	if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, m_commandBuffersCompute.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
	else {
		std::cout << "" << std::endl;
		std::cout << "Success: created command buffer" << std::endl;
	}
}

void VoxelEngine::createTextureRessources()
{
	m_textureImageView = createImageView(m_textureImage, VK_FORMAT_R8G8B8A8_SNORM, VK_IMAGE_ASPECT_COLOR_BIT);

	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 0;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	vkCreateSampler(m_logicalDevice, &samplerInfo, nullptr, &m_textureSampler);
}

void VoxelEngine::drawFrameCompute()
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	// Compute submission        
	vkWaitForFences(m_logicalDevice, 1, &m_computeInFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

	updateUniformBuffer(m_currentFrame);

	vkResetFences(m_logicalDevice, 1, &m_computeInFlightFences[m_currentFrame]);

	vkResetCommandBuffer(m_commandBuffersCompute[m_currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
	recordCommandBufferCompute(m_commandBuffersCompute[m_currentFrame]);

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffersCompute[m_currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_computeFinishedSemaphores[m_currentFrame];

	if (vkQueueSubmit(m_queueCompute, 1, &submitInfo, m_computeInFlightFences[m_currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit compute command buffer!");
	};

	// Graphics submission
	vkWaitForFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_logicalDevice, m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	vkResetFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrame]);

	vkResetCommandBuffer(m_commandBuffers[m_currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
	recordCommandBuffer(m_commandBuffers[m_currentFrame], imageIndex, m_descriptorSetsCompute);

	VkSemaphore waitSemaphores[2];
	waitSemaphores[1] = m_imageAvailableSemaphores[m_currentFrame];
	waitSemaphores[0] = m_computeFinishedSemaphores[m_currentFrame];

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submitInfo.waitSemaphoreCount = 2;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_renderFinishedSemaphores[m_currentFrame];

	if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_renderFinishedSemaphores[m_currentFrame];

	VkSwapchainKHR swapChains[] = { m_swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VoxelEngine::recordCommandBufferCompute(VkCommandBuffer a_commandBuffer)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(a_commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording compute command buffer!");
	}

	vkCmdBindPipeline(a_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelineCompute);

	vkCmdBindDescriptorSets(a_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelineLayoutCompute, 0, 1, &m_descriptorSetsCompute[m_currentFrame], 0, nullptr);

	vkCmdDispatch(a_commandBuffer, WIDTH, HEIGHT, 1);

	vkCmdDispatch(a_commandBuffer, ceil(WIDTH/8), ceil(HEIGHT/4), 1);

	if (vkEndCommandBuffer(a_commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record compute command buffer!");
	}
}

#pragma endregion





