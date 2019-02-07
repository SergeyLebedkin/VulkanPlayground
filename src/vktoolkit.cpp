#include "vktoolkit.hpp"
#include <cassert>
#include <array>

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
	applicationInfo.apiVersion = VK_API_VERSION_1_0;

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
	VulkanInstance& instance)
{
	// destroy handles
	vkDestroyInstance(instance.instance, VK_NULL_HANDLE);
	// clear handles
	instance.physicalDevices.clear();
	instance.instance = VK_NULL_HANDLE;
}

// vulkanDeviceCreate
void vulkanDeviceCreate(
	VulkanInstance&            instance,
	VkPhysicalDeviceType       physicalDeviceType,
	VkPhysicalDeviceFeatures&  physicalDeviceFeatures,
	std::vector<const char *>& enabledExtensionNames,
	VulkanDevice*              device)
{
	// check handles
	assert(device);

	// find physical device by type
	device->physicalDevice = VK_NULL_HANDLE;
	for (const auto& physicalDevice : instance.physicalDevices)
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

	// get queue family properties
	device->queueFamilyPropertiesGraphics = queueFamilyProperties[device->queueFamilyIndexGraphics];
	device->queueFamilyPropertiesCompute = queueFamilyProperties[device->queueFamilyIndexCompute];
	device->queueFamilyPropertiesTransfer = queueFamilyProperties[device->queueFamilyIndexTransfer];

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
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = device->queueFamilyIndexGraphics;
	VKT_CHECK(vkCreateCommandPool(device->device, &commandPoolCreateInfo, VK_NULL_HANDLE, &device->commandPool));
	assert(device->commandPool);
}

// vulkanDeviceDestroy
void vulkanDeviceDestroy(
	VulkanDevice& device)
{
	// destroy handles
	vkDestroyCommandPool(device.device, device.commandPool, VK_NULL_HANDLE);
	vmaDestroyAllocator(device.allocator);
	vkDestroyDevice(device.device, VK_NULL_HANDLE);
	// clear handles
	device.commandPool = VK_NULL_HANDLE;
	device.allocator = VK_NULL_HANDLE;
	device.queueTransfer = VK_NULL_HANDLE;
	device.queueCompute = VK_NULL_HANDLE;
	device.queueGraphics = VK_NULL_HANDLE;
	device.device = VK_NULL_HANDLE;
	device.queueFamilyIndexTransfer = UINT32_MAX;
	device.queueFamilyIndexCompute = UINT32_MAX;
	device.queueFamilyIndexGraphics = UINT32_MAX;
	device.queueFamilyPropertiesGraphics = {};
	device.queueFamilyPropertiesCompute = {};
	device.queueFamilyPropertiesTransfer = {};
	device.physicalDeviceMemoryProperties = {};
	device.physicalDeviceProperties = {};
	device.physicalDeviceFeatures = {};
	device.physicalDevice = VK_NULL_HANDLE;
}

// vulkanSamplerCreate
void vulkanSamplerCreate(
	VulkanDevice& device,
	VkFilter filter,
	VkSamplerAddressMode samplerAddressMode,
	VkBool32 anisotropyEnable,
	VulkanSampler* sampler)
{
	// check handles
	assert(sampler);

	// VkSamplerCreateInfo
	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext = VK_NULL_HANDLE;
	samplerCreateInfo.flags = 0;
	samplerCreateInfo.magFilter = filter;
	samplerCreateInfo.minFilter = filter;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = samplerAddressMode;
	samplerCreateInfo.addressModeV = samplerAddressMode;
	samplerCreateInfo.addressModeW = samplerAddressMode;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = anisotropyEnable;
	samplerCreateInfo.maxAnisotropy = device.physicalDeviceProperties.limits.maxSamplerAnisotropy;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 1.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	VKT_CHECK(vkCreateSampler(device.device, &samplerCreateInfo, VK_NULL_HANDLE, &sampler->sampler));
	assert(sampler->sampler);
}

// vulkanSamplerDestroy
void vulkanSamplerDestroy(
	VulkanDevice& device,
	VulkanSampler& sampler)
{
	// destroy handles
	vkDestroySampler(device.device, sampler.sampler, VK_NULL_HANDLE);
	// clear handles
	sampler.sampler = VK_NULL_HANDLE;
}

// vulkanImageViewCreate
void vulkanImageViewCreate(
	VulkanDevice& device,
	VulkanImage& image,
	VkFormat format,
	VulkanImageView* imageView)
{
	// check handles
	assert(imageView);

	// get image view type
	VkImageViewType imageViewType =
		image.depth > 1 ? VK_IMAGE_VIEW_TYPE_3D : 
		image.height > 1 ? VK_IMAGE_VIEW_TYPE_2D : 
		VK_IMAGE_VIEW_TYPE_1D;

	// VkImageViewCreateInfo
	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = VK_NULL_HANDLE;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.image = image.image;
	imageViewCreateInfo.viewType = imageViewType;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = image.mipLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	VKT_CHECK(vkCreateImageView(device.device, &imageViewCreateInfo, VK_NULL_HANDLE, &imageView->imageView));
	assert(imageView->imageView);
}

// vulkanImageViewDestroy
void vulkanImageViewDestroy(
	VulkanDevice& device, 
	VulkanImageView& imageView)
{
	// destroy handles
	vkDestroyImageView(device.device, imageView.imageView, VK_NULL_HANDLE);
	// clear handles
	imageView.imageView = VK_NULL_HANDLE;
}

// vulkanImageCreate
void vulkanImageCreate(
	VulkanDevice& device,
	VkImageUsageFlags usage,
	VkFormat format,
	uint32_t width,
	uint32_t height,
	uint32_t depth,
	VulkanImage* image)
{
	// check parameters
	assert(width);
	assert(height);
	assert(depth);
	assert(image);

	// get mipmap levels count and image type
	uint32_t mipLevels = (uint32_t)std::floor(std::log2(std::max(width, std::max(height, depth)))) + 1;
	VkImageType imageType = depth > 1 ? VK_IMAGE_TYPE_3D : height > 1 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_1D;

	// store properties
	image->imageType = imageType;
	image->format = format;
	image->width = width;
	image->height = height;
	image->depth = depth;
	image->mipLevels = mipLevels;
	image->currentLayouts.clear();
	image->currentLayouts.resize(mipLevels, VK_IMAGE_LAYOUT_UNDEFINED);
	image->finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	// VkImageCreateInfo
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = VK_NULL_HANDLE;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = imageType;
	imageCreateInfo.format = format;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = depth;
	imageCreateInfo.mipLevels = mipLevels;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | usage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
	imageCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	// VmaAllocationCreateInfo
	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocCreateInfo.flags = 0;

	// vmaCreateImage
	VKT_CHECK(vmaCreateImage(device.allocator, &imageCreateInfo, &allocCreateInfo, &image->image, &image->allocation, &image->allocationInfo));
	assert(image->allocation);
	assert(image->image);
}

// vulkanImageRead
void vulkanImageRead(
	VulkanDevice& device,
	VulkanImage&  image,
	uint32_t      mipLevel,
	void*         data)
{
	// check parameters
	assert(image.width);
	assert(image.height);
	assert(image.depth);
	assert(mipLevel < image.mipLevels);
	assert(data);

	// calculate mipmap sizes 
	uint32_t scaleFactor = (uint32_t)std::pow(2, mipLevel);
	uint32_t width = std::max(1U, image.width / scaleFactor);
	uint32_t height = std::max(1U, image.height / scaleFactor);
	uint32_t depth = std::max(1U, image.depth / scaleFactor);

	// create staging image
	VulkanImage staginImage{};
	staginImage.allocation = VK_NULL_HANDLE;
	staginImage.allocationInfo = {};
	staginImage.image = VK_NULL_HANDLE;
	staginImage.imageType = image.imageType;
	staginImage.format = image.format;
	staginImage.width = width;
	staginImage.height = height;
	staginImage.depth = depth;
	staginImage.mipLevels = 1;
	staginImage.currentLayouts.resize(1, VK_IMAGE_LAYOUT_UNDEFINED);
	staginImage.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	// VkImageCreateInfo
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = VK_NULL_HANDLE;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = staginImage.imageType;
	imageCreateInfo.format = staginImage.format;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = depth;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
	imageCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	// VmaAllocationCreateInfo
	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
	allocCreateInfo.flags = 0;

	// vmaCreateImage
	VKT_CHECK(vmaCreateImage(device.allocator, &imageCreateInfo, &allocCreateInfo, &staginImage.image, &staginImage.allocation, &staginImage.allocationInfo));
	assert(staginImage.image);
	assert(staginImage.allocation);

	// flush data to image
	vulkanImageCopy(device, image, mipLevel, staginImage, 0);

	// fill data to image
	void* mappedData = nullptr;
	VKT_CHECK(vmaMapMemory(device.allocator, staginImage.allocation, &mappedData));
	assert(mappedData);
	memcpy(data, mappedData, (size_t)staginImage.allocationInfo.size);
	vmaUnmapMemory(device.allocator, staginImage.allocation);

	// destroy handles
	vmaDestroyImage(device.allocator, staginImage.image, staginImage.allocation);
}

// vulkanImageWrite
void vulkanImageWrite(
	VulkanDevice& device,
	VulkanImage&  image,
	uint32_t      mipLevel,
	const void*   data)
{
	// check parameters
	assert(image.width);
	assert(image.height);
	assert(image.depth);
	assert(mipLevel < image.mipLevels);
	assert(data);

	// calculate mipmap sizes 
	uint32_t scaleFactor = (uint32_t)std::pow(2, mipLevel);
	uint32_t width = std::max(1U, image.width / scaleFactor);
	uint32_t height = std::max(1U, image.height / scaleFactor);
	uint32_t depth = std::max(1U, image.depth / scaleFactor);

	// create staging image
	VulkanImage staginImage{};
	staginImage.allocation = VK_NULL_HANDLE;
	staginImage.image = VK_NULL_HANDLE;
	staginImage.allocationInfo = {};
	staginImage.imageType = image.imageType;
	staginImage.format = image.format;
	staginImage.width = width;
	staginImage.height = height;
	staginImage.depth = depth;
	staginImage.mipLevels = 1;
	staginImage.currentLayouts.resize(1, VK_IMAGE_LAYOUT_PREINITIALIZED);
	staginImage.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

	// VkImageCreateInfo
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = VK_NULL_HANDLE;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = staginImage.imageType;
	imageCreateInfo.format = staginImage.format;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = depth;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
	imageCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

	// VmaAllocationCreateInfo
	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	allocCreateInfo.flags = 0;

	// vmaCreateImage
	VKT_CHECK(vmaCreateImage(device.allocator, &imageCreateInfo, &allocCreateInfo, &staginImage.image, &staginImage.allocation, &staginImage.allocationInfo));
	assert(staginImage.image);
	assert(staginImage.allocation);

	// fill data to image
	void* mappedData = nullptr;
	VKT_CHECK(vmaMapMemory(device.allocator, staginImage.allocation, &mappedData));
	assert(mappedData);
	memcpy(mappedData, data, (size_t)staginImage.allocationInfo.size);
	vmaUnmapMemory(device.allocator, staginImage.allocation);

	// flush data to image
	vulkanImageCopy(device, staginImage, 0, image, mipLevel);

	// destroy handles
	vmaDestroyImage(device.allocator, staginImage.image, staginImage.allocation);
}

// vulkanImageCopy
void vulkanImageCopy(
	VulkanDevice& device,
	VulkanImage&  imageSrc,
	uint32_t      mipLevelSrc,
	VulkanImage&  imageDst,
	uint32_t      mipLevelDst)
{
	// check parameters
	assert(mipLevelSrc < imageSrc.mipLevels);
	assert(mipLevelDst < imageDst.mipLevels);
	assert(imageSrc.currentLayouts[mipLevelSrc] != VK_IMAGE_LAYOUT_UNDEFINED);

	// calculate mipmap sizes 
	uint32_t scaleFactorSrc = (uint32_t)std::pow(2, mipLevelSrc);
	uint32_t scaleFactorDst = (uint32_t)std::pow(2, mipLevelDst);
	uint32_t widthSrc = std::max(1U, imageSrc.width / scaleFactorSrc);
	uint32_t widthDst = std::max(1U, imageDst.width / scaleFactorDst);
	uint32_t heightSrc = std::max(1U, imageSrc.height / scaleFactorSrc);
	uint32_t heightDst = std::max(1U, imageDst.height / scaleFactorDst);
	uint32_t depthSrc = std::max(1U, imageSrc.depth / scaleFactorSrc);
	uint32_t depthDst = std::max(1U, imageDst.depth / scaleFactorDst);

	// check parameters
	assert(widthSrc == widthDst);
	assert(heightSrc == heightDst);
	assert(depthSrc == depthDst);

	// VkCommandBufferAllocateInfo
	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = VK_NULL_HANDLE;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandPool = device.commandPool;
	commandBufferAllocateInfo.commandBufferCount = 1;

	// VkCommandBuffer
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	VKT_CHECK(vkAllocateCommandBuffers(device.device, &commandBufferAllocateInfo, &commandBuffer));
	assert(commandBuffer);

	// VkCommandBufferBeginInfo
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = VK_NULL_HANDLE;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional
	VKT_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

	// VkImageMemoryBarrier
	VkImageMemoryBarrier imgMemBarrier{};
	imgMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imgMemBarrier.pNext = VK_NULL_HANDLE;
	imgMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imgMemBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imgMemBarrier.subresourceRange.levelCount = 1;
	imgMemBarrier.subresourceRange.baseArrayLayer = 0;
	imgMemBarrier.subresourceRange.layerCount = 1;

	imgMemBarrier.subresourceRange.baseMipLevel = mipLevelSrc;
	imgMemBarrier.srcAccessMask = 0;
	imgMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	imgMemBarrier.oldLayout = imageSrc.currentLayouts[mipLevelSrc];
	imgMemBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	imgMemBarrier.image = imageSrc.image;
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imgMemBarrier);
	imageSrc.currentLayouts[mipLevelSrc] = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

	imgMemBarrier.subresourceRange.baseMipLevel = mipLevelDst;
	imgMemBarrier.srcAccessMask = 0;
	imgMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imgMemBarrier.oldLayout = imageDst.currentLayouts[mipLevelDst];
	imgMemBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imgMemBarrier.image = imageDst.image;
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imgMemBarrier);
	imageDst.currentLayouts[mipLevelDst] = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	// VkImageCopy
	VkImageCopy imageCopy{};
	imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.srcSubresource.mipLevel = mipLevelSrc;
	imageCopy.srcSubresource.baseArrayLayer = 0;
	imageCopy.srcSubresource.layerCount = 1;
	imageCopy.srcOffset.x = 0;
	imageCopy.srcOffset.y = 0;
	imageCopy.srcOffset.z = 0;
	imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.dstSubresource.mipLevel = mipLevelDst;
	imageCopy.dstSubresource.baseArrayLayer = 0;
	imageCopy.dstSubresource.layerCount = 1;
	imageCopy.dstOffset.x = 0;
	imageCopy.dstOffset.y = 0;
	imageCopy.dstOffset.z = 0;
	imageCopy.extent.width = widthSrc;
	imageCopy.extent.height = heightSrc;
	imageCopy.extent.depth = depthSrc;
	vkCmdCopyImage(commandBuffer, imageSrc.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, imageDst.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

	imgMemBarrier.subresourceRange.baseMipLevel = mipLevelSrc;
	imgMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imgMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imgMemBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	imgMemBarrier.newLayout = imageSrc.finalLayout;
	imgMemBarrier.image = imageSrc.image;
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imgMemBarrier);
	imageSrc.currentLayouts[mipLevelSrc] = imageSrc.finalLayout;

	imgMemBarrier.subresourceRange.baseMipLevel = mipLevelDst;
	imgMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imgMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imgMemBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imgMemBarrier.newLayout = imageDst.finalLayout;
	imgMemBarrier.image = imageDst.image;
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imgMemBarrier);
	imageDst.currentLayouts[mipLevelDst] = imageDst.finalLayout;

	// vkEndCommandBuffer
	VKT_CHECK(vkEndCommandBuffer(commandBuffer));

	// submit and wait
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = VK_NULL_HANDLE;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	VKT_CHECK(vkQueueSubmit(device.queueGraphics, 1, &submitInfo, VK_NULL_HANDLE));
	VKT_CHECK(vkQueueWaitIdle(device.queueGraphics));

	// free command buffer
	vkFreeCommandBuffers(device.device, device.commandPool, 1, &commandBuffer);
}

// vulkanImageBuildMipmaps
void vulkanImageBuildMipmaps(
	VulkanDevice& device,
	VulkanImage& image)
{
	throw std::runtime_error("Not implemented");
}

// vulkanImageDestroy
void vulkanImageDestroy(
	VulkanDevice& device,
	VulkanImage& image)
{
	// destroy handles
	vmaDestroyImage(device.allocator, image.image, image.allocation);
	// clear handles
	image.allocation = VK_NULL_HANDLE;
	image.allocationInfo = {};
	image.image = VK_NULL_HANDLE;
	image.imageType = VK_IMAGE_TYPE_1D;
	image.format = VK_FORMAT_UNDEFINED;
	image.width = 0;
	image.height = 0;
	image.depth = 0;
	image.mipLevels = 0;
	image.currentLayouts = {};
	image.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
}

// vulkanBufferCreate
void vulkanBufferCreate(
	VulkanDevice& device,
	VkBufferUsageFlags usage,
	VmaMemoryUsage memoryUsage,
	VkDeviceSize size,
	VulkanBuffer* buffer)
{
	// check size
	assert(size);
	assert(buffer);

	// store properties
	buffer->size = size;

	// VkBufferCreateInfo
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = VK_NULL_HANDLE;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;

	// VmaAllocationCreateInfo
	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = memoryUsage;
	allocCreateInfo.flags = 0;

	// vmaCreateBuffer
	VKT_CHECK(vmaCreateBuffer(device.allocator, &bufferCreateInfo, &allocCreateInfo, &buffer->buffer, &buffer->allocation, &buffer->allocationInfo));
	assert(buffer->allocation);
	assert(buffer->buffer);
}

// vulkanBufferRead
void vulkanBufferRead(
	VulkanDevice& device,
	VulkanBuffer& buffer,
	VkDeviceSize offset,
	VkDeviceSize size,
	void* data)
{
	// check data
	assert(data);

	// get memory buffer properties
	VmaAllocationInfo allocationInfo{};
	vmaGetAllocationInfo(device.allocator, buffer.allocation, &allocationInfo);
	VkMemoryPropertyFlags memFlags{};
	vmaGetMemoryTypeProperties(device.allocator, allocationInfo.memoryType, &memFlags);

	// if target device memory is host visible, then just map/unmap memory to device memory
	if ((memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
		void* mappedData = nullptr;
		VKT_CHECK(vmaMapMemory(device.allocator, buffer.allocation, &mappedData));
		assert(mappedData);
		memcpy(data, (uint8_t *)mappedData + offset, (size_t)size);
		vmaUnmapMemory(device.allocator, buffer.allocation);
	}
	else // if target device memory is NOT host visible, then we need use staging buffer
	{
		// create staging buffer and memory
		VulkanBuffer stagingBuffer{};
		vulkanBufferCreate(device, 0, VMA_MEMORY_USAGE_GPU_TO_CPU, size, &stagingBuffer);

		// copy buffers
		vulkanBufferCopy(device, buffer, offset, stagingBuffer, 0, size);

		// map staging buffer and memory
		void* mappedData = nullptr;
		VKT_CHECK(vmaMapMemory(device.allocator, stagingBuffer.allocation, &mappedData));
		assert(mappedData);
		memcpy(data, mappedData, (size_t)size);
		vmaUnmapMemory(device.allocator, stagingBuffer.allocation);

		// destroy buffer and free memory
		vulkanBufferDestroy(device, stagingBuffer);
	}
}

// vulkanBufferWrite
void vulkanBufferWrite(
	VulkanDevice& device,
	VulkanBuffer& buffer,
	VkDeviceSize offset,
	VkDeviceSize size,
	const void* data)
{
	// check data
	assert(data);

	// get memory buffer properties
	VmaAllocationInfo allocationInfo{};
	vmaGetAllocationInfo(device.allocator, buffer.allocation, &allocationInfo);
	VkMemoryPropertyFlags memFlags;
	vmaGetMemoryTypeProperties(device.allocator, allocationInfo.memoryType, &memFlags);

	// if target device memory is host visible, then just map/unmap memory to device memory
	if ((memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
		void* mappedData = nullptr;
		VKT_CHECK(vmaMapMemory(device.allocator, buffer.allocation, &mappedData));
		assert(mappedData);
		memcpy((uint8_t *)mappedData + offset, data, (size_t)size);
		vmaUnmapMemory(device.allocator, buffer.allocation);
	}
	else // if target device memory is NOT host visible, then we need use staging buffer
	{
		// create staging buffer and memory
		VulkanBuffer stagingBuffer{};
		vulkanBufferCreate(device, 0, VMA_MEMORY_USAGE_CPU_TO_GPU, size, &stagingBuffer);

		// map staging buffer and memory
		void* mappedData = nullptr;
		VKT_CHECK(vmaMapMemory(device.allocator, stagingBuffer.allocation, &mappedData));
		assert(mappedData);
		memcpy(mappedData, data, (size_t)size);
		vmaUnmapMemory(device.allocator, stagingBuffer.allocation);

		// copy buffers
		vulkanBufferCopy(device, stagingBuffer, 0, buffer, offset, size);

		// destroy buffer and free memory
		vulkanBufferDestroy(device, stagingBuffer);
	}
}

// vulkanBufferCopy
void vulkanBufferCopy(
	VulkanDevice& device,
	VulkanBuffer& bufferSrc,
	VkDeviceSize  offsetSrc,
	VulkanBuffer& bufferDst,
	VkDeviceSize  offsetDst,
	VkDeviceSize size)
{
	// VkCommandBufferAllocateInfo
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = VK_NULL_HANDLE;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandPool = device.commandPool;
	commandBufferAllocateInfo.commandBufferCount = 1;
	VKT_CHECK(vkAllocateCommandBuffers(device.device, &commandBufferAllocateInfo, &commandBuffer));
	assert(commandBuffer);

	// VkCommandBufferBeginInfo
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = VK_NULL_HANDLE;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional
	VKT_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

	// VkBufferCopy
	VkBufferCopy bufferCopy{};
	bufferCopy.srcOffset = offsetSrc;
	bufferCopy.dstOffset = offsetDst;
	bufferCopy.size = size;
	vkCmdCopyBuffer(commandBuffer, bufferSrc.buffer, bufferDst.buffer, 1, &bufferCopy);

	// vkEndCommandBuffer
	VKT_CHECK(vkEndCommandBuffer(commandBuffer));

	// submit and wait
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = VK_NULL_HANDLE;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	VKT_CHECK(vkQueueSubmit(device.queueGraphics, 1, &submitInfo, VK_NULL_HANDLE));
	VKT_CHECK(vkQueueWaitIdle(device.queueGraphics));

	// free command buffer
	vkFreeCommandBuffers(device.device, device.commandPool, 1, &commandBuffer);
}


// vulkanBufferDestroy
void vulkanBufferDestroy(
	VulkanDevice& device,
	VulkanBuffer& buffer)
{
	// destroy handles
	vmaDestroyBuffer(device.allocator, buffer.buffer, buffer.allocation);
	// clear handles
	buffer.allocation = VK_NULL_HANDLE;
	buffer.buffer = VK_NULL_HANDLE;
}

// vulkanCommandBufferAllocate(
void vulkanCommandBufferAllocate(
	VulkanDevice&        device,
	VkCommandBufferLevel commandBufferLevel,
	VulkanCommandBuffer* commandBuffer)
{
	// check handles
	assert(commandBuffer);
	// VkCommandBufferAllocateInfo
	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = VK_NULL_HANDLE;
	commandBufferAllocateInfo.commandPool = device.commandPool;
	commandBufferAllocateInfo.level = commandBufferLevel;
	commandBufferAllocateInfo.commandBufferCount = 1;
	VKT_CHECK(vkAllocateCommandBuffers(device.device, &commandBufferAllocateInfo, &commandBuffer->ñommandBuffer));
	assert(commandBuffer->ñommandBuffer);
}

// vulkanCommandBufferFree
void vulkanCommandBufferFree(
	VulkanDevice& device, 
	VulkanCommandBuffer& commandBuffer)
{
	// free handles
	vkFreeCommandBuffers(device.device, device.commandPool, 1, &commandBuffer.ñommandBuffer);
	// clear handles
	commandBuffer.ñommandBuffer = VK_NULL_HANDLE;
}

// vulkanSemaphoreCreate
void vulkanSemaphoreCreate(
	VulkanDevice& device, 
	VulkanSemaphore* semaphore)
{
	// check handles
	assert(semaphore);
	// VkSemaphoreCreateInfo
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = VK_NULL_HANDLE;
	semaphoreCreateInfo.flags = 0;
	VKT_CHECK(vkCreateSemaphore(device.device, &semaphoreCreateInfo, VK_NULL_HANDLE, &semaphore->semaphore));
	assert(semaphore->semaphore);
}

// vulkanSemaphoreDestroy
void vulkanSemaphoreDestroy(
	VulkanDevice& device, 
	VulkanSemaphore& semaphore)
{
	// destroy handles
	vkDestroySemaphore(device.device, semaphore.semaphore, VK_NULL_HANDLE);
	// clear handles
	semaphore.semaphore = VK_NULL_HANDLE;
}

// vulkanSwapchainCreate
void vulkanSwapchainCreate(
	VulkanDevice& device,
	VulkanSurface& surface,
	VulkanSwapchain* swapchain)
{
	// check handles
	assert(swapchain);

	// find surface format, present mode and capabilities 
	swapchain->surfaceFormat = vulkanGetDefaultSurfaceFormat(device, surface);
	swapchain->presentMode = vulkanGetDefaultSurfacePresentMode(device, surface);
	VKT_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physicalDevice, surface.surface, &swapchain->surfaceCapabilities));

	// get present supported
	VkBool32 supported = VK_FALSE;
	vkGetPhysicalDeviceSurfaceSupportKHR(device.physicalDevice, device.queueFamilyIndexGraphics, surface.surface, &supported);
	assert(supported);

	// VkSwapchainCreateInfoKHR
	VkSwapchainCreateInfoKHR swapchainCreateInfoKHR{};
	swapchainCreateInfoKHR.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfoKHR.pNext = VK_NULL_HANDLE;
	swapchainCreateInfoKHR.flags = 0;
	swapchainCreateInfoKHR.surface = surface.surface;
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
	VKT_CHECK(vkCreateSwapchainKHR(device.device, &swapchainCreateInfoKHR, nullptr, &swapchain->swapchain));
	assert(swapchain->swapchain);

	// VkAttachmentDescription - color
	std::array<VkAttachmentDescription, 2> attachmentDescriptions;
	// color attachment
	attachmentDescriptions[0].flags = 0;
	attachmentDescriptions[0].format = swapchain->surfaceFormat.format;
	attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// depth-stencil attachment
	attachmentDescriptions[1].flags = 0;
	attachmentDescriptions[1].format = VK_FORMAT_D24_UNORM_S8_UINT;
	attachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// VkAttachmentReference - color
	std::array<VkAttachmentReference, 1> colorAttachmentReferences;
	colorAttachmentReferences[0].attachment = 0;
	colorAttachmentReferences[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// VkAttachmentReference - depth-stencil
	VkAttachmentReference depthStencilAttachmentReference{};
	depthStencilAttachmentReference.attachment = 1;
	depthStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// VkSubpassDescription - subpassDescriptions
	std::array<VkSubpassDescription, 1> subpassDescriptions;
	subpassDescriptions[0].flags = 0;
	subpassDescriptions[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescriptions[0].inputAttachmentCount = 0;
	subpassDescriptions[0].pInputAttachments = VK_NULL_HANDLE;
	subpassDescriptions[0].colorAttachmentCount = (uint32_t)colorAttachmentReferences.size();
	subpassDescriptions[0].pColorAttachments = colorAttachmentReferences.data();
	subpassDescriptions[0].pResolveAttachments = VK_NULL_HANDLE;
	subpassDescriptions[0].pDepthStencilAttachment = &depthStencilAttachmentReference;
	subpassDescriptions[0].preserveAttachmentCount = 0;
	subpassDescriptions[0].pPreserveAttachments = VK_NULL_HANDLE;

	// VkRenderPassCreateInfo
	VkRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = (uint32_t)attachmentDescriptions.size();
	renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
	renderPassCreateInfo.subpassCount = (uint32_t)subpassDescriptions.size();
	renderPassCreateInfo.pSubpasses = subpassDescriptions.data();

	// vkCreateRenderPass
	VKT_CHECK(vkCreateRenderPass(device.device, &renderPassCreateInfo, VK_NULL_HANDLE, &swapchain->renderPass));

	// VkImageCreateInfo - depth and stencil
	VkImageCreateInfo imageCreateInfoDS{};
	imageCreateInfoDS.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfoDS.pNext = VK_NULL_HANDLE;
	imageCreateInfoDS.flags = 0;
	imageCreateInfoDS.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfoDS.format = VK_FORMAT_D24_UNORM_S8_UINT; // must be the same as imageViewCreateInfoDS.format (see below)
	imageCreateInfoDS.extent.width = swapchain->surfaceCapabilities.currentExtent.width;
	imageCreateInfoDS.extent.height = swapchain->surfaceCapabilities.currentExtent.height;
	imageCreateInfoDS.extent.depth = 1;
	imageCreateInfoDS.mipLevels = 1;
	imageCreateInfoDS.arrayLayers = 1;
	imageCreateInfoDS.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfoDS.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfoDS.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageCreateInfoDS.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfoDS.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
	imageCreateInfoDS.pQueueFamilyIndices = VK_NULL_HANDLE;
	imageCreateInfoDS.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	// VmaAllocationCreateInfo
	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocCreateInfo.flags = 0;

	// vmaCreateImage
	VKT_CHECK(vmaCreateImage(device.allocator, &imageCreateInfoDS, &allocCreateInfo, &swapchain->imageDS, &swapchain->allocationDS, VK_NULL_HANDLE));
	assert(swapchain->imageDS);
	assert(swapchain->allocationDS);

	// VkImageViewCreateInfo
	VkImageViewCreateInfo imageViewCreateInfoDS{};
	imageViewCreateInfoDS.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfoDS.pNext = VK_NULL_HANDLE;
	imageViewCreateInfoDS.flags = 0;
	imageViewCreateInfoDS.image = swapchain->imageDS;
	imageViewCreateInfoDS.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfoDS.format = VK_FORMAT_D24_UNORM_S8_UINT; // must be the same as imageCreateInfoDS.format (see upper)
	imageViewCreateInfoDS.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfoDS.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfoDS.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfoDS.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfoDS.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	imageViewCreateInfoDS.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfoDS.subresourceRange.levelCount = 1;
	imageViewCreateInfoDS.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfoDS.subresourceRange.layerCount = 1;
	VKT_CHECK(vkCreateImageView(device.device, &imageViewCreateInfoDS, VK_NULL_HANDLE, &swapchain->imageViewDS));
	assert(swapchain->imageViewDS);

	// swapChainImages
	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(device.device, swapchain->swapchain, &imageCount, nullptr);
	swapchain->images.resize(imageCount);
	vkGetSwapchainImagesKHR(device.device, swapchain->swapchain, &imageCount, swapchain->images.data());

	// create image views
	swapchain->imageViews.clear();
	swapchain->imageViews.reserve(imageCount);
	for (const auto& image : swapchain->images) {
		// VkImageViewCreateInfo
		VkImageView imageView = VK_NULL_HANDLE;
		VkImageViewCreateInfo imageViewCreateInfoDS{};
		imageViewCreateInfoDS.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfoDS.pNext = VK_NULL_HANDLE;
		imageViewCreateInfoDS.flags = 0;
		imageViewCreateInfoDS.image = image;
		imageViewCreateInfoDS.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfoDS.format = swapchain->surfaceFormat.format;
		imageViewCreateInfoDS.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfoDS.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfoDS.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfoDS.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfoDS.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfoDS.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfoDS.subresourceRange.levelCount = 1;
		imageViewCreateInfoDS.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfoDS.subresourceRange.layerCount = 1;
		VKT_CHECK(vkCreateImageView(device.device, &imageViewCreateInfoDS, VK_NULL_HANDLE, &imageView));
		assert(imageView);

		// add image view
		swapchain->imageViews.push_back(imageView);
	}

	// create image views
	swapchain->framebuffers.clear();
	swapchain->framebuffers.reserve(imageCount);
	for (const auto& imageView : swapchain->imageViews) {
		// image views
		VkImageView imageViews[] = { imageView, swapchain->imageViewDS };

		// VkFramebufferCreateInfo 
		VkFramebuffer framebuffer = VK_NULL_HANDLE;
		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = VK_NULL_HANDLE;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = swapchain->renderPass;
		framebufferCreateInfo.attachmentCount = VKT_ARRAY_ELEMENTS_COUNT(imageViews);
		framebufferCreateInfo.pAttachments = imageViews;
		framebufferCreateInfo.width = swapchain->surfaceCapabilities.currentExtent.width;
		framebufferCreateInfo.height = swapchain->surfaceCapabilities.currentExtent.height;
		framebufferCreateInfo.layers = 1;
		VKT_CHECK(vkCreateFramebuffer(device.device, &framebufferCreateInfo, VK_NULL_HANDLE, &framebuffer));

		// add image view
		swapchain->framebuffers.push_back(framebuffer);
	}
}

// vulkanSwapchainDestroy
void vulkanSwapchainDestroy(
	VulkanDevice& device,
	VulkanSwapchain& swapchain)
{
	// destroy handles
	for (const auto& framebuffer : swapchain.framebuffers)
		vkDestroyFramebuffer(device.device, framebuffer, VK_NULL_HANDLE);
	for (const auto& imageView : swapchain.imageViews)
		vkDestroyImageView(device.device, imageView, VK_NULL_HANDLE);
	vkDestroyImageView(device.device, swapchain.imageViewDS, VK_NULL_HANDLE);
	vmaDestroyImage(device.allocator, swapchain.imageDS, swapchain.allocationDS);
	vkDestroyRenderPass(device.device, swapchain.renderPass, VK_NULL_HANDLE);
	vkDestroySwapchainKHR(device.device, swapchain.swapchain, VK_NULL_HANDLE);
	// clear handles
	swapchain.framebuffers.clear();
	swapchain.imageViews.clear();
	swapchain.images.clear();
	swapchain.imageViewDS = VK_NULL_HANDLE;
	swapchain.imageDS = VK_NULL_HANDLE;
	swapchain.allocationDS = VK_NULL_HANDLE;
	swapchain.renderPass = VK_NULL_HANDLE;
	swapchain.swapchain = VK_NULL_HANDLE;
	swapchain.surfaceCapabilities = {};
	swapchain.presentMode = {};
	swapchain.surfaceFormat = {};
}

// vulkanSwapchainBeginFrame
void vulkanSwapchainBeginFrame(
	VulkanDevice& device,
	VulkanSwapchain& swapchain,
	VulkanSemaphore& semaphore,
	uint32_t* frameIndex)
{
	// check handles
	assert(frameIndex);
	// vkAcquireNextImageKHR
	VKT_CHECK(vkAcquireNextImageKHR(device.device, swapchain.swapchain, UINT64_MAX, semaphore.semaphore, VK_NULL_HANDLE, frameIndex));
}

// vulkanSwapchainEndFrame
void vulkanSwapchainEndFrame(
	VulkanDevice& device,
	VulkanSwapchain& swapchain,
	VulkanSemaphore& semaphore,
	uint32_t frameIndex)
{
	// VkPresentInfoKHR
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &semaphore.semaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain.swapchain;
	presentInfo.pImageIndices = &frameIndex;
	presentInfo.pResults = nullptr; // Optional
	VKT_CHECK(vkQueuePresentKHR(device.queueGraphics, &presentInfo));
	VKT_CHECK(vkQueueWaitIdle(device.queueGraphics));
}

// vulkanInitDeviceQueueCreateInfo
VkDeviceQueueCreateInfo vulkanInitDeviceQueueCreateInfo(
	uint32_t queueFamilyIndex,
	uint32_t queueCount,
	const float* pQueuePriorities)
{
	// check handles
	assert(pQueuePriorities);
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
	VulkanDevice& device,
	VulkanSurface& surface)
{
	// get queue family properties count
	uint32_t formatsCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device.physicalDevice, surface.surface, &formatsCount, nullptr);
	assert(formatsCount);
	// get surface formats list
	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device.physicalDevice, surface.surface, &formatsCount, surfaceFormats.data());

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
	VulkanDevice& device,
	VulkanSurface& surface)
{
	// get present modes count
	uint32_t presentModesCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device.physicalDevice, surface.surface, &presentModesCount, nullptr);
	assert(presentModesCount);
	// get present modes list
	std::vector<VkPresentModeKHR> presentModes(presentModesCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device.physicalDevice, surface.surface, &presentModesCount, presentModes.data());

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

// vulkanQueueSubmit
void vulkanQueueSubmit(
	VulkanDevice& device,
	VulkanCommandBuffer& commandBuffer,
	VulkanSemaphore& waitSemaphore,
	VulkanSemaphore& signalSemaphore)
{
	// VkSubmitInfo
	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &waitSemaphore.semaphore;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &signalSemaphore.semaphore;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer.ñommandBuffer;
	VKT_CHECK(vkQueueSubmit(device.queueGraphics, 1, &submitInfo, VK_NULL_HANDLE));
}
