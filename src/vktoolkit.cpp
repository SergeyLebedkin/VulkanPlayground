#include "vktoolkit.hpp"
#include <cassert>

// vulkanInstanceCreate
void vulkanInstanceCreate(
	std::vector<const char *>& enabledLayerNames,
	std::vector<const char *>& enabledExtensionNames,
	VulkanInstance* instance)
{
	// check handles
	assert(instance);

	// VkApplicationInfo
	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = VK_NULL_HANDLE;
	applicationInfo.pApplicationName = "Vulkan toolkit app";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.pEngineName = "Vulkan toolkit engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	applicationInfo.apiVersion = VK_VERSION_1_1;

	// VkInstanceCreateInfo
	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = VK_NULL_HANDLE;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledLayerCount = (uint32_t)enabledLayerNames.size();
	instanceCreateInfo.ppEnabledLayerNames = enabledLayerNames.data();
	instanceCreateInfo.enabledExtensionCount = (uint32_t)enabledExtensionNames.size();
	instanceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();

	// vkCreateInstance
	VKT_CHECK(vkCreateInstance(&instanceCreateInfo, VK_NULL_HANDLE, &instance->instance));
	assert(instance->instance);

	// get physical devices count
	uint32_t physicalDevicesCount = 0;
	VKT_CHECK(vkEnumeratePhysicalDevices(instance->instance, &physicalDevicesCount, nullptr));
	assert(physicalDevicesCount);
	// get physical devices list
	instance->physicalDevices.resize(physicalDevicesCount);
	VKT_CHECK(vkEnumeratePhysicalDevices(instance->instance, &physicalDevicesCount, instance->physicalDevices.data()));
}

// vulkanInstanceDestroy
void vulkanInstanceDestroy(
	VulkanInstance* instance)
{
	// check handles
	assert(instance);
	// destroy handles
	vkDestroyInstance(instance->instance, VK_NULL_HANDLE);
	// clear handles
	instance->physicalDevices.clear();
	instance->instance = VK_NULL_HANDLE;
}

// vulkanDeviceCreate
void vulkanDeviceCreate(
	VulkanInstance*            instance,
	VkPhysicalDeviceType       physicalDeviceType,
	VkPhysicalDeviceFeatures&  physicalDeviceFeatures,
	std::vector<const char *>& enabledExtensionNames,
	VulkanDevice*              device)
{
	// check handles
	assert(instance);
	assert(device);

	// find physical device by type
	device->physicalDevice = VK_NULL_HANDLE;
	for (const auto& physicalDevice : instance->physicalDevices)
	{
		VkPhysicalDeviceFeatures physicalDeviceFeatures;
		VkPhysicalDeviceProperties physicalDeviceProperties;
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
		vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);
		if (physicalDeviceProperties.deviceType & physicalDeviceType)
		{
			device->physicalDevice = physicalDevice;
			device->physicalDeviceFeatures = physicalDeviceFeatures;
			device->physicalDeviceProperties = physicalDeviceProperties;
			device->physicalDeviceMemoryProperties = physicalDeviceMemoryProperties;
			break;
		}
	}
	assert(device->physicalDevice);

	// get queue family properties count
	uint32_t queueFamilyPropertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device->physicalDevice, &queueFamilyPropertiesCount, nullptr);
	assert(queueFamilyPropertiesCount);
	// get queue family properties list
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device->physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

	// find queue family compute, graphics, transfer
	device->queueFamilyIndexCompute = vulkanFindQueueFamilyPropertiesByFlags(queueFamilyProperties, VK_QUEUE_COMPUTE_BIT);
	device->queueFamilyIndexGraphics = vulkanFindQueueFamilyPropertiesByFlags(queueFamilyProperties, VK_QUEUE_GRAPHICS_BIT);
	device->queueFamilyIndexTransfer = vulkanFindQueueFamilyPropertiesByFlags(queueFamilyProperties, VK_QUEUE_TRANSFER_BIT);
	assert(device->queueFamilyIndexCompute < UINT32_MAX);
	assert(device->queueFamilyIndexGraphics < UINT32_MAX);
	assert(device->queueFamilyIndexTransfer < UINT32_MAX);

	// deviceQueueCreateInfos
	std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
	float queuePriorities[] = { 1.0f };

	// VkDeviceQueueCreateInfo - graphics
	VkDeviceQueueCreateInfo deviceQueueCreateInfoGraphics = vulkanInitDeviceQueueCreateInfo(device->queueFamilyIndexGraphics, 1, queuePriorities);
	deviceQueueCreateInfos.push_back(deviceQueueCreateInfoGraphics);

	// VkDeviceQueueCreateInfo - compute
	VkDeviceQueueCreateInfo deviceQueueCreateInfoCompute = vulkanInitDeviceQueueCreateInfo(device->queueFamilyIndexCompute, 1, queuePriorities);
	if (device->queueFamilyIndexGraphics != device->queueFamilyIndexCompute)
		deviceQueueCreateInfos.push_back(deviceQueueCreateInfoCompute);

	// VkDeviceQueueCreateInfo - transfer
	VkDeviceQueueCreateInfo deviceQueueCreateInfoTransfer = vulkanInitDeviceQueueCreateInfo(device->queueFamilyIndexTransfer, 1, queuePriorities);
	if (device->queueFamilyIndexGraphics != device->queueFamilyIndexTransfer)
		deviceQueueCreateInfos.push_back(deviceQueueCreateInfoTransfer);

	// VkDeviceCreateInfo
	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = VK_NULL_HANDLE;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = (uint32_t)deviceQueueCreateInfos.size();
	deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
	deviceCreateInfo.enabledExtensionCount = (uint32_t)enabledExtensionNames.size();
	deviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = VK_NULL_HANDLE;
	deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
	VKT_CHECK(vkCreateDevice(device->physicalDevice, &deviceCreateInfo, VK_NULL_HANDLE, &device->device));
	assert(device->device);

	// vkGetDeviceQueue
	vkGetDeviceQueue(device->device, device->queueFamilyIndexGraphics, 0, &device->queueGraphics);
	vkGetDeviceQueue(device->device, device->queueFamilyIndexCompute, 0, &device->queueCompute);
	vkGetDeviceQueue(device->device, device->queueFamilyIndexTransfer, 0, &device->queueTransfer);
	assert(device->queueGraphics);
	assert(device->queueCompute);
	assert(device->queueTransfer);

	// VmaAllocatorCreateInfo
	VmaAllocatorCreateInfo allocatorCreateInfo{};
	allocatorCreateInfo.flags = 0;
	allocatorCreateInfo.physicalDevice = device->physicalDevice;
	allocatorCreateInfo.device = device->device;
	VKT_CHECK(vmaCreateAllocator(&allocatorCreateInfo, &device->allocator));
	assert(device->allocator);

	// VkCommandPoolCreateInfo
	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.pNext = VK_NULL_HANDLE;
	commandPoolCreateInfo.flags = 0;
	commandPoolCreateInfo.queueFamilyIndex = device->queueFamilyIndexGraphics;
	VKT_CHECK(vkCreateCommandPool(device->device, &commandPoolCreateInfo, VK_NULL_HANDLE, &device->commandPool));
	assert(device->commandPool);
}

// vulkanDeviceDestroy
void vulkanDeviceDestroy(
	VulkanDevice* device)
{
	// check handles
	assert(device);
	// destroy handles
	vkDestroyCommandPool(device->device, device->commandPool, VK_NULL_HANDLE);
	vmaDestroyAllocator(device->allocator);
	vkDestroyDevice(device->device, VK_NULL_HANDLE);
	// clear handles
	device->commandPool = VK_NULL_HANDLE;
	device->allocator = VK_NULL_HANDLE;
	device->queueTransfer = VK_NULL_HANDLE;
	device->queueCompute = VK_NULL_HANDLE;
	device->queueGraphics = VK_NULL_HANDLE;
	device->device = VK_NULL_HANDLE;
	device->queueFamilyIndexTransfer = UINT32_MAX;
	device->queueFamilyIndexCompute = UINT32_MAX;
	device->queueFamilyIndexGraphics = UINT32_MAX;
	device->physicalDeviceMemoryProperties = {};
	device->physicalDeviceProperties = {};
	device->physicalDeviceFeatures = {};
	device->physicalDevice = VK_NULL_HANDLE;
}

// vulkanSwapchainCreate
void vulkanSwapchainCreate(
	VulkanDevice* device,
	VulkanSurface* surface,
	VulkanSwapchain* swapchain)
{
	// check handles
	assert(device);
	assert(surface);
	assert(swapchain);

	// find surface format, present mode and capabilities 
	swapchain->surfaceFormat = vulkanGetDefaultSurfaceFormat(device, surface);
	swapchain->presentMode = vulkanGetDefaultSurfacePresentMode(device, surface);
	VKT_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physicalDevice, surface->surface, &swapchain->surfaceCapabilities));

	// get present supported
	VkBool32 supported = VK_FALSE;
	vkGetPhysicalDeviceSurfaceSupportKHR(device->physicalDevice, device->queueFamilyIndexGraphics, surface->surface, &supported);
	assert(supported);

	// VkSwapchainCreateInfoKHR
	VkSwapchainCreateInfoKHR swapchainCreateInfoKHR{};
	swapchainCreateInfoKHR.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfoKHR.pNext = VK_NULL_HANDLE;
	swapchainCreateInfoKHR.flags = 0;
	swapchainCreateInfoKHR.surface = surface->surface;
	swapchainCreateInfoKHR.minImageCount = 3;
	swapchainCreateInfoKHR.imageFormat = swapchain->surfaceFormat.format;
	swapchainCreateInfoKHR.imageColorSpace = swapchain->surfaceFormat.colorSpace;
	swapchainCreateInfoKHR.imageExtent.width = swapchain->surfaceCapabilities.currentExtent.width;
	swapchainCreateInfoKHR.imageExtent.height = swapchain->surfaceCapabilities.currentExtent.height;
	swapchainCreateInfoKHR.imageArrayLayers = 1;
	swapchainCreateInfoKHR.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfoKHR.queueFamilyIndexCount = 0;
	swapchainCreateInfoKHR.pQueueFamilyIndices = VK_NULL_HANDLE;
	swapchainCreateInfoKHR.preTransform = swapchain->surfaceCapabilities.currentTransform;
	swapchainCreateInfoKHR.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfoKHR.presentMode = swapchain->presentMode;
	swapchainCreateInfoKHR.clipped = VK_TRUE;
	swapchainCreateInfoKHR.oldSwapchain = VK_NULL_HANDLE;
	VKT_CHECK(vkCreateSwapchainKHR(device->device, &swapchainCreateInfoKHR, nullptr, &swapchain->swapchain));
	assert(swapchain->swapchain);

	// VkImageCreateInfo - depth and stencil
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = VK_NULL_HANDLE;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	imageCreateInfo.extent.width = swapchain->surfaceCapabilities.currentExtent.width;
	imageCreateInfo.extent.height = swapchain->surfaceCapabilities.currentExtent.width;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
	imageCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	// VmaAllocationCreateInfo
	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocCreateInfo.flags = 0;

	// vmaCreateImage
	VKT_CHECK(vmaCreateImage(device->allocator, &imageCreateInfo, &allocCreateInfo, &swapchain->imageDS, &swapchain->allocationDS, VK_NULL_HANDLE));
	assert(swapchain->imageDS);
	assert(swapchain->allocationDS);

	// VkImageViewCreateInfo
	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = VK_NULL_HANDLE;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.image = swapchain->imageDS;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	VKT_CHECK(vkCreateImageView(device->device, &imageViewCreateInfo, VK_NULL_HANDLE, &swapchain->imageViewDS));
	assert(swapchain->imageViewDS);
}

// vulkanSwapchainDestroy
void vulkanSwapchainDestroy(
	VulkanDevice* device,
	VulkanSwapchain* swapchain)
{
	// check handles
	assert(device);
	assert(swapchain);
	// destroy handles
	vkDestroyImageView(device->device, swapchain->imageViewDS, VK_NULL_HANDLE);
	vmaDestroyImage(device->allocator, swapchain->imageDS, swapchain->allocationDS);
	vkDestroySwapchainKHR(device->device, swapchain->swapchain, VK_NULL_HANDLE);
	// clear handles
	swapchain->imageViewDS = VK_NULL_HANDLE;
	swapchain->imageDS = VK_NULL_HANDLE;
	swapchain->allocationDS = VK_NULL_HANDLE;
	swapchain->swapchain = VK_NULL_HANDLE;
	swapchain->surfaceCapabilities = {};
	swapchain->presentMode = {};
	swapchain->surfaceFormat = {};
}

// vulkanInitDeviceQueueCreateInfo
VkDeviceQueueCreateInfo vulkanInitDeviceQueueCreateInfo(
	uint32_t queueFamilyIndex,
	uint32_t queueCount,
	const float * pQueuePriorities)
{
	// VkDeviceQueueCreateInfo
	VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.pNext = VK_NULL_HANDLE;
	deviceQueueCreateInfo.flags = 0;
	deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
	deviceQueueCreateInfo.queueCount = queueCount;
	deviceQueueCreateInfo.pQueuePriorities = pQueuePriorities;
	return deviceQueueCreateInfo;
}

// vulkanGetDefaultSurfaceFormat
VkSurfaceFormatKHR vulkanGetDefaultSurfaceFormat(
	VulkanDevice* device,
	VulkanSurface* surface)
{
	// check handles
	assert(device);
	assert(surface);

	// get queue family properties count
	uint32_t formatsCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice, surface->surface, &formatsCount, nullptr);
	assert(formatsCount);
	// get surface formats list
	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice, surface->surface, &formatsCount, surfaceFormats.data());

	// get default surface format
	if ((surfaceFormats.size() == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED)) {
		VkSurfaceFormatKHR format;
		format.format = VK_FORMAT_R8G8B8A8_SNORM;
		format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		return format;
	};

	// try to find HDR format
	for (const auto& surfaceFormat : surfaceFormats)
		if ((surfaceFormat.format == VK_FORMAT_A2R10G10B10_SNORM_PACK32) || (surfaceFormat.format == VK_FORMAT_A2B10G10R10_SNORM_PACK32))
			return surfaceFormat;

	// try to find standard format
	for (const auto& surfaceFormat : surfaceFormats)
		if ((surfaceFormat.format == VK_FORMAT_R8G8B8A8_SNORM) || (surfaceFormat.format == VK_FORMAT_B8G8R8_SNORM))
			return surfaceFormat;

	// return default
	return surfaceFormats[0];
}

// vulkanGetDefaultSurfacePresentMode
VkPresentModeKHR vulkanGetDefaultSurfacePresentMode(
	VulkanDevice* device, 
	VulkanSurface* surface)
{
	// check handles
	assert(device);
	assert(surface);

	// get present modes count
	uint32_t presentModesCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice, surface->surface, &presentModesCount, nullptr);
	assert(presentModesCount);
	// get present modes list
	std::vector<VkPresentModeKHR> presentModes(presentModesCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice, surface->surface, &presentModesCount, presentModes.data());

	// try to find MAILBOX mode
	for (const auto& presentMode : presentModes)
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return presentMode;

	// try to find FIFO mode
	for (const auto& presentMode : presentModes)
		if (presentMode == VK_PRESENT_MODE_FIFO_KHR)
			return presentMode;

	// return default
	return presentModes[0];
}

// vulkanFindQueueFamilyPropertiesByFlags
uint32_t vulkanFindQueueFamilyPropertiesByFlags(
	std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
	VkQueueFlags queueFlags)
{
	for (size_t i = 0; i < queueFamilyProperties.size(); i++)
		if ((queueFamilyProperties[i].queueFlags & queueFlags) == queueFlags)
			return (uint32_t)i;
	assert(0);
	return UINT32_MAX;
}
