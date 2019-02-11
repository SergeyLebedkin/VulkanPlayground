#include "vktoolkit.hpp"
#include <cassert>
#include <fstream>
#include <array>
#include <map>

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
	applicationInfo.apiVersion = VK_API_VERSION_1_1;

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
	samplerCreateInfo.maxLod = FLT_MAX;
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

// vulkanImageCreate
void vulkanImageCreate(
	VulkanDevice& device,
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
	VkImageViewType imageViewType = depth > 1 ? VK_IMAGE_VIEW_TYPE_3D : height > 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_1D;

	// store properties
	image->imageType = imageType;
	image->format = format;
	image->width = width;
	image->height = height;
	image->depth = depth;
	image->mipLevels = mipLevels;
	image->accessFlags.clear();
	image->accessFlags.resize(mipLevels, 0);
	image->imageLayouts.clear();
	image->imageLayouts.resize(mipLevels, VK_IMAGE_LAYOUT_UNDEFINED);

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
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
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

	// VkImageViewCreateInfo
	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = VK_NULL_HANDLE;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.image = image->image;
	imageViewCreateInfo.viewType = imageViewType;
	imageViewCreateInfo.format = image->format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = image->mipLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	VKT_CHECK(vkCreateImageView(device.device, &imageViewCreateInfo, VK_NULL_HANDLE, &image->imageView));
	assert(image->imageView);
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
	uint32_t width = std::max(1U, image.width >> mipLevel);
	uint32_t height = std::max(1U, image.height >> mipLevel);
	uint32_t depth = std::max(1U, image.depth >> mipLevel);

	// create staging image
	VulkanImage imageStaging{};
	imageStaging.allocation = VK_NULL_HANDLE;
	imageStaging.allocationInfo = {};
	imageStaging.image = VK_NULL_HANDLE;
	imageStaging.imageView = VK_NULL_HANDLE;
	imageStaging.imageType = image.imageType;
	imageStaging.format = image.format;
	imageStaging.width = width;
	imageStaging.height = height;
	imageStaging.depth = depth;
	imageStaging.mipLevels = 1;
	imageStaging.accessFlags.resize(1, 0);
	imageStaging.imageLayouts.resize(1, VK_IMAGE_LAYOUT_UNDEFINED);

	// VkImageCreateInfo
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = VK_NULL_HANDLE;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = imageStaging.imageType;
	imageCreateInfo.format = imageStaging.format;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = depth;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
	imageCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	// VmaAllocationCreateInfo
	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
	allocCreateInfo.flags = 0;

	// vmaCreateImage
	VKT_CHECK(vmaCreateImage(device.allocator, &imageCreateInfo, &allocCreateInfo, &imageStaging.image, &imageStaging.allocation, &imageStaging.allocationInfo));
	assert(imageStaging.image);
	assert(imageStaging.allocation);

	// create command buffer
	VulkanCommandBuffer commandBuffer{};
	vulkanCommandBufferAllocate(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &commandBuffer);
	vulkanCommandBufferBegin(device, commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// change image layouts
	vulkanImageSetLayout(commandBuffer, image, mipLevel, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vulkanImageSetLayout(commandBuffer, imageStaging, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// VkImageCopy
	VkImageCopy imageCopy{};
	imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.srcSubresource.mipLevel = mipLevel;
	imageCopy.srcSubresource.layerCount = 1;
	imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.dstSubresource.mipLevel = 0;
	imageCopy.dstSubresource.layerCount = 1;
	imageCopy.extent.width = width;
	imageCopy.extent.height = height;
	imageCopy.extent.depth = depth;
	vkCmdCopyImage(commandBuffer.commandBuffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, imageStaging.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

	// change image layouts
	vulkanImageSetLayout(commandBuffer, image, mipLevel, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	vulkanImageSetLayout(commandBuffer, imageStaging, 0, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// vkEndCommandBuffer
	vulkanCommandBufferEnd(commandBuffer);

	// submit and wait
	vulkanQueueSubmit(device, commandBuffer, nullptr, nullptr);
	VKT_CHECK(vkQueueWaitIdle(device.queueGraphics));

	// command buffer free
	vulkanCommandBufferFree(device, commandBuffer);

	// fill data to image
	void* mappedData = nullptr;
	VKT_CHECK(vmaMapMemory(device.allocator, imageStaging.allocation, &mappedData));
	assert(mappedData);
	memcpy(data, mappedData, (size_t)imageStaging.allocationInfo.size);
	vmaUnmapMemory(device.allocator, imageStaging.allocation);

	// destroy handles
	vmaDestroyImage(device.allocator, imageStaging.image, imageStaging.allocation);
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
	uint32_t width = std::max(1U, image.width >> mipLevel);
	uint32_t height = std::max(1U, image.height >> mipLevel);
	uint32_t depth = std::max(1U, image.depth >> mipLevel);

	// create staging image
	VulkanImage imageStaging{};
	imageStaging.allocation = VK_NULL_HANDLE;
	imageStaging.allocationInfo = {};
	imageStaging.image = VK_NULL_HANDLE;
	imageStaging.imageView = VK_NULL_HANDLE;
	imageStaging.imageType = image.imageType;
	imageStaging.format = image.format;
	imageStaging.width = width;
	imageStaging.height = height;
	imageStaging.depth = depth;
	imageStaging.mipLevels = 1;
	imageStaging.accessFlags.resize(1, 0);
	imageStaging.imageLayouts.resize(1, VK_IMAGE_LAYOUT_PREINITIALIZED);

	// VkImageCreateInfo
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = VK_NULL_HANDLE;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = imageStaging.imageType;
	imageCreateInfo.format = imageStaging.format;
	imageCreateInfo.extent.width = imageStaging.width;
	imageCreateInfo.extent.height = imageStaging.height;
	imageCreateInfo.extent.depth = imageStaging.depth;
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
	VKT_CHECK(vmaCreateImage(device.allocator, &imageCreateInfo, &allocCreateInfo, &imageStaging.image, &imageStaging.allocation, &imageStaging.allocationInfo));
	assert(imageStaging.image);
	assert(imageStaging.allocation);

	// fill data to image
	void* mappedData = nullptr;
	VKT_CHECK(vmaMapMemory(device.allocator, imageStaging.allocation, &mappedData));
	assert(mappedData);
	memcpy(mappedData, data, (size_t)imageStaging.allocationInfo.size);
	vmaUnmapMemory(device.allocator, imageStaging.allocation);

	// create command buffer
	VulkanCommandBuffer commandBuffer{};
	vulkanCommandBufferAllocate(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &commandBuffer);
	vulkanCommandBufferBegin(device, commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// change image layouts
	vulkanImageSetLayout(commandBuffer, imageStaging, 0, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vulkanImageSetLayout(commandBuffer, image, mipLevel, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// VkImageCopy
	VkImageCopy imageCopy{};
	imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.srcSubresource.mipLevel = 0;
	imageCopy.srcSubresource.layerCount = 1;
	imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.dstSubresource.mipLevel = mipLevel;
	imageCopy.dstSubresource.layerCount = 1;
	imageCopy.extent.width = width;
	imageCopy.extent.height = height;
	imageCopy.extent.depth = depth;
	vkCmdCopyImage(commandBuffer.commandBuffer, imageStaging.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

	// change image layouts
	vulkanImageSetLayout(commandBuffer, imageStaging, 0, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vulkanImageSetLayout(commandBuffer, image, mipLevel, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	// vkEndCommandBuffer
	vulkanCommandBufferEnd(commandBuffer);

	// submit and wait
	vulkanQueueSubmit(device, commandBuffer, nullptr, nullptr);
	VKT_CHECK(vkQueueWaitIdle(device.queueGraphics));

	// command buffer free
	vulkanCommandBufferFree(device, commandBuffer);

	// destroy handles
	vmaDestroyImage(device.allocator, imageStaging.image, imageStaging.allocation);
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
	assert(imageSrc.imageLayouts[mipLevelSrc] != VK_IMAGE_LAYOUT_UNDEFINED);

	// calculate mipmap sizes 
	uint32_t widthSrc = std::max(1U, imageSrc.width >> mipLevelSrc);
	uint32_t widthDst = std::max(1U, imageDst.width >> mipLevelDst);
	uint32_t heightSrc = std::max(1U, imageSrc.height >> mipLevelSrc);
	uint32_t heightDst = std::max(1U, imageDst.height >> mipLevelDst);
	uint32_t depthSrc = std::max(1U, imageSrc.depth >> mipLevelSrc);
	uint32_t depthDst = std::max(1U, imageDst.depth >> mipLevelDst);

	// check parameters
	assert(widthSrc == widthDst);
	assert(heightSrc == heightDst);
	assert(depthSrc == depthDst);

	// create command buffer
	VulkanCommandBuffer commandBuffer{};
	vulkanCommandBufferAllocate(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &commandBuffer);
	vulkanCommandBufferBegin(device, commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// change image layouts
	vulkanImageSetLayout(commandBuffer, imageSrc, mipLevelSrc, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vulkanImageSetLayout(commandBuffer, imageDst, mipLevelDst, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// VkImageCopy
	VkImageCopy imageCopy{};
	imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.srcSubresource.mipLevel = mipLevelSrc;
	imageCopy.srcSubresource.layerCount = 1;
	imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.dstSubresource.mipLevel = mipLevelDst;
	imageCopy.dstSubresource.layerCount = 1;
	imageCopy.extent.width = widthSrc;
	imageCopy.extent.height = heightSrc;
	imageCopy.extent.depth = depthSrc;
	vkCmdCopyImage(commandBuffer.commandBuffer, imageSrc.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, imageDst.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

	// change image layouts
	vulkanImageSetLayout(commandBuffer, imageSrc, mipLevelSrc, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	vulkanImageSetLayout(commandBuffer, imageDst, mipLevelDst, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	// vkEndCommandBuffer
	vulkanCommandBufferEnd(commandBuffer);

	// submit and wait
	vulkanQueueSubmit(device, commandBuffer, nullptr, nullptr);
	VKT_CHECK(vkQueueWaitIdle(device.queueGraphics));

	// free command buffer
	vulkanCommandBufferFree(device, commandBuffer);
}

// vulkanImageBuildMipmaps
void vulkanImageBuildMipmaps(
	VulkanDevice& device,
	VulkanImage&  image)
{
	// check parameters
	assert(image.imageLayouts[0] != VK_IMAGE_LAYOUT_UNDEFINED);

	// create command buffer
	VulkanCommandBuffer commandBuffer{};
	vulkanCommandBufferAllocate(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &commandBuffer);
	vulkanCommandBufferBegin(device, commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// set mipmap level 0 to transfer source optimal
	vulkanImageSetLayout(commandBuffer, image, 0, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	// copy levels
	for (uint32_t mipLevel = 1; mipLevel < image.mipLevels; mipLevel++) {
		// get next mipmap level extent
		int32_t width = std::max(1U, image.width >> mipLevel);
		int32_t height = std::max(1U, image.height >> mipLevel);
		int32_t depth = std::max(1U, image.depth >> mipLevel);

		// change image layouts
		vulkanImageSetLayout(commandBuffer, image, mipLevel, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		// VkImageBlit
		VkImageBlit imageBlit = {};
		imageBlit.srcOffsets[0] = { 0, 0, 0 };
		imageBlit.srcOffsets[1] = { (int32_t)image.width, (int32_t)image.height, (int32_t)image.depth };
		imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.srcSubresource.mipLevel = 0;
		imageBlit.srcSubresource.baseArrayLayer = 0;
		imageBlit.srcSubresource.layerCount = 1;
		imageBlit.dstOffsets[0] = { 0, 0, 0 };
		imageBlit.dstOffsets[1] = { width, height, depth };
		imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.dstSubresource.mipLevel = mipLevel;
		imageBlit.dstSubresource.baseArrayLayer = 0;
		imageBlit.dstSubresource.layerCount = 1;
		vkCmdBlitImage(commandBuffer.commandBuffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_NEAREST);

		// change image layouts
		vulkanImageSetLayout(commandBuffer, image, mipLevel, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	// set mipmap level 0 to shader read optimal
	vulkanImageSetLayout(commandBuffer, image, 0, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	// vkEndCommandBuffer
	vulkanCommandBufferEnd(commandBuffer);

	// submit and wait
	vulkanQueueSubmit(device, commandBuffer, nullptr, nullptr);
	VKT_CHECK(vkQueueWaitIdle(device.queueGraphics));

	// free command buffer
	vulkanCommandBufferFree(device, commandBuffer);
}

// vulkanImageSetLayout
void vulkanImageSetLayout(
	VulkanCommandBuffer& commandBuffer,
	VulkanImage&         image,
	uint32_t             mipLevel,
	VkAccessFlags        accessFlags,
	VkImageLayout        imageLayout)
{
	// check parameters
	assert(mipLevel < image.mipLevels);
	// VkImageMemoryBarrier
	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = VK_NULL_HANDLE;
	imageMemoryBarrier.srcAccessMask = image.accessFlags[mipLevel];
	imageMemoryBarrier.dstAccessMask = accessFlags;
	imageMemoryBarrier.oldLayout = image.imageLayouts[mipLevel];
	imageMemoryBarrier.newLayout = imageLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image.image;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.baseMipLevel = mipLevel;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;
	vkCmdPipelineBarrier(commandBuffer.commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	image.accessFlags[mipLevel] = accessFlags;
	image.imageLayouts[mipLevel] = imageLayout;
}

// vulkanImageDestroy
void vulkanImageDestroy(
	VulkanDevice& device,
	VulkanImage&  image)
{
	// destroy handles
	vkDestroyImageView(device.device, image.imageView, VK_NULL_HANDLE);
	vmaDestroyImage(device.allocator, image.image, image.allocation);
	// clear handles
	image.allocation = VK_NULL_HANDLE;
	image.allocationInfo = {};
	image.image = VK_NULL_HANDLE;
	image.imageView = VK_NULL_HANDLE;
	image.imageType = VK_IMAGE_TYPE_1D;
	image.format = VK_FORMAT_UNDEFINED;
	image.width = 0;
	image.height = 0;
	image.depth = 0;
	image.mipLevels = 0;
	image.accessFlags = {};
	image.imageLayouts = {};
}

// vulkanBufferCreate
void vulkanBufferCreate(
	VulkanDevice&      device,
	VkBufferUsageFlags usage,
	VkDeviceSize       size,
	VulkanBuffer*      buffer)
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
	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
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
	VkDeviceSize  offset,
	VkDeviceSize  size,
	void* data)
{
	// check data
	assert(data);

	// create staging buffer and memory
	VulkanBuffer stagingBuffer{};
	stagingBuffer.size = size;

	// VkBufferCreateInfo
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = VK_NULL_HANDLE;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;

	// VmaAllocationCreateInfo
	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
	allocCreateInfo.flags = 0;

	// vmaCreateBuffer
	VKT_CHECK(vmaCreateBuffer(device.allocator, &bufferCreateInfo, &allocCreateInfo, &stagingBuffer.buffer, &stagingBuffer.allocation, &stagingBuffer.allocationInfo));
	assert(stagingBuffer.allocation);
	assert(stagingBuffer.buffer);

	// copy buffers
	vulkanBufferCopy(device, buffer, offset, stagingBuffer, 0, size);

	// map staging buffer and memory
	void* mappedData = nullptr;
	VKT_CHECK(vmaMapMemory(device.allocator, stagingBuffer.allocation, &mappedData));
	assert(mappedData);
	memcpy(data, mappedData, (size_t)size);
	vmaUnmapMemory(device.allocator, stagingBuffer.allocation);

	// destroy buffer and free memory
	vmaDestroyBuffer(device.allocator, stagingBuffer.buffer, stagingBuffer.allocation);
}

// vulkanBufferWrite
void vulkanBufferWrite(
	VulkanDevice& device,
	VulkanBuffer& buffer,
	VkDeviceSize  offset,
	VkDeviceSize  size,
	const void*   data)
{
	// check data
	assert(data);

	// create staging buffer and memory
	VulkanBuffer stagingBuffer{};
	stagingBuffer.size = size;

	// VkBufferCreateInfo
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = VK_NULL_HANDLE;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;

	// VmaAllocationCreateInfo
	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	allocCreateInfo.flags = 0;

	// vmaCreateBuffer
	VKT_CHECK(vmaCreateBuffer(device.allocator, &bufferCreateInfo, &allocCreateInfo, &stagingBuffer.buffer, &stagingBuffer.allocation, &stagingBuffer.allocationInfo));
	assert(stagingBuffer.allocation);
	assert(stagingBuffer.buffer);

	// map staging buffer and memory
	void* mappedData = nullptr;
	VKT_CHECK(vmaMapMemory(device.allocator, stagingBuffer.allocation, &mappedData));
	assert(mappedData);
	memcpy(mappedData, data, (size_t)size);
	vmaUnmapMemory(device.allocator, stagingBuffer.allocation);

	// copy buffers
	vulkanBufferCopy(device, stagingBuffer, 0, buffer, offset, size);

	// destroy buffer and free memory
	vmaDestroyBuffer(device.allocator, stagingBuffer.buffer, stagingBuffer.allocation);
}

// vulkanBufferCopy
void vulkanBufferCopy(
	VulkanDevice& device,
	VulkanBuffer& bufferSrc,
	VkDeviceSize  offsetSrc,
	VulkanBuffer& bufferDst,
	VkDeviceSize  offsetDst,
	VkDeviceSize  size)
{
	// create command buffer
	VulkanCommandBuffer commandBuffer{};
	vulkanCommandBufferAllocate(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &commandBuffer);
	vulkanCommandBufferBegin(device, commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// VkBufferCopy
	VkBufferCopy bufferCopy{};
	bufferCopy.srcOffset = offsetSrc;
	bufferCopy.dstOffset = offsetDst;
	bufferCopy.size = size;
	vkCmdCopyBuffer(commandBuffer.commandBuffer, bufferSrc.buffer, bufferDst.buffer, 1, &bufferCopy);

	// vkEndCommandBuffer
	vulkanCommandBufferEnd(commandBuffer);

	// submit and wait
	vulkanQueueSubmit(device, commandBuffer, nullptr, nullptr);
	VKT_CHECK(vkQueueWaitIdle(device.queueGraphics));

	// free command buffer
	vulkanCommandBufferFree(device, commandBuffer);
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
	VKT_CHECK(vkAllocateCommandBuffers(device.device, &commandBufferAllocateInfo, &commandBuffer->commandBuffer));
	assert(commandBuffer->commandBuffer);
}

// vulkanCommandBufferBegin
void vulkanCommandBufferBegin(
	VulkanDevice& device,
	VulkanCommandBuffer& commandBuffer,
	VkCommandBufferUsageFlags flags)
{
	// VkCommandBufferBeginInfo
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = VK_NULL_HANDLE;
	commandBufferBeginInfo.flags = flags;
	commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional
	VKT_CHECK(vkBeginCommandBuffer(commandBuffer.commandBuffer, &commandBufferBeginInfo));
}

// vulkanCommandBufferEnd
void vulkanCommandBufferEnd(
	VulkanCommandBuffer& commandBuffer)
{
	VKT_CHECK(vkEndCommandBuffer(commandBuffer.commandBuffer));
}

// vulkanCommandBufferFree
void vulkanCommandBufferFree(
	VulkanDevice& device, 
	VulkanCommandBuffer& commandBuffer)
{
	// free handles
	vkFreeCommandBuffers(device.device, device.commandPool, 1, &commandBuffer.commandBuffer);
	// clear handles
	commandBuffer.commandBuffer = VK_NULL_HANDLE;
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
	swapchainCreateInfoKHR.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
	swapchainCreateInfoKHR.pQueueFamilyIndices = VK_NULL_HANDLE;
	swapchainCreateInfoKHR.preTransform = swapchain->surfaceCapabilities.currentTransform;
	swapchainCreateInfoKHR.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfoKHR.presentMode = swapchain->presentMode;
	swapchainCreateInfoKHR.clipped = VK_TRUE;
	swapchainCreateInfoKHR.oldSwapchain = VK_NULL_HANDLE;
	VKT_CHECK(vkCreateSwapchainKHR(device.device, &swapchainCreateInfoKHR, VK_NULL_HANDLE, &swapchain->swapchain));
	assert(swapchain->swapchain);

	// VkAttachmentDescription - color
	std::array<VkAttachmentDescription, 2> attachmentDescriptions;
	// color attachment
	attachmentDescriptions[0].flags = 0;
	attachmentDescriptions[0].format = swapchain->surfaceFormat.format;
	attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// depth-stencil attachment
	attachmentDescriptions[1].flags = 0;
	attachmentDescriptions[1].format = VK_FORMAT_D24_UNORM_S8_UINT;
	attachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
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

// loadFileData
void loadFileData(
	const char*        fileName,
	std::vector<char>& data)
{
	// open file
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	assert(file.is_open());

	// get file size
	auto fileSize = file.tellg();
	assert(fileSize > 0);

	// read data and close
	data.resize((size_t)fileSize);
	file.seekg(0, std::ios::beg);
	file.read(data.data(), fileSize);
	file.close();
}

// vulkanPipelineCreate
void vulkanPipelineCreate(
	VulkanDevice&                       device,
	VkRenderPass                        renderPass,
	uint32_t                            subpass,
	const char*                         fileNameVS,
	const char*                         fileNameFS,
	uint32_t                            vertexInputBindingDescriptionCount,
	VkVertexInputBindingDescription     vertexInputBindingDescriptions[],
	uint32_t                            vertexInputAttributeDescriptionCount,
	VkVertexInputAttributeDescription   vertexInputAttributeDescriptions[],
	uint32_t                            descriptorSetLayoutBindingCount,
	VkDescriptorSetLayoutBinding        descriptorSetLayoutBindings[],
	uint32_t                            pipelineColorBlendAttachmentStateCount,
	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentStates[],
	VulkanPipeline*                     pipeline)
{
	// check handles
	assert(fileNameVS);
	assert(fileNameFS);
	assert(vertexInputBindingDescriptions);
	assert(vertexInputAttributeDescriptions);
	assert(descriptorSetLayoutBindings);
	assert(pipelineColorBlendAttachmentStates);
	assert(pipeline);

	// VkShaderModuleCreateInfo - vertex shader
	std::vector<char> dataVS;
	loadFileData(fileNameVS, dataVS);
	VkShaderModuleCreateInfo shaderModuleCreateInfoVS{};
	shaderModuleCreateInfoVS.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfoVS.pNext = VK_NULL_HANDLE;
	shaderModuleCreateInfoVS.flags = VK_NULL_HANDLE;
	shaderModuleCreateInfoVS.codeSize = (uint32_t)dataVS.size();
	shaderModuleCreateInfoVS.pCode = (uint32_t *)dataVS.data();
	VKT_CHECK(vkCreateShaderModule(device.device, &shaderModuleCreateInfoVS, VK_NULL_HANDLE, &pipeline->shaderModuleVS));
	assert(pipeline->shaderModuleVS);

	// VkShaderModuleCreateInfo - fragment shader
	std::vector<char> dataFS;
	loadFileData(fileNameFS, dataFS);
	VkShaderModuleCreateInfo shaderModuleCreateInfoFS{};
	shaderModuleCreateInfoFS.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfoFS.pNext = VK_NULL_HANDLE;
	shaderModuleCreateInfoFS.flags = VK_NULL_HANDLE;
	shaderModuleCreateInfoFS.codeSize = (uint32_t)dataFS.size();
	shaderModuleCreateInfoFS.pCode = (uint32_t *)dataFS.data();
	VKT_CHECK(vkCreateShaderModule(device.device, &shaderModuleCreateInfoFS, VK_NULL_HANDLE, &pipeline->shaderModuleFS));
	assert(pipeline->shaderModuleFS);

	// VkPipelineShaderStageCreateInfo
	std::array<VkPipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfos;
	// vertex shader
	pipelineShaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfos[0].pNext = VK_NULL_HANDLE;
	pipelineShaderStageCreateInfos[0].flags = 0;
	pipelineShaderStageCreateInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	pipelineShaderStageCreateInfos[0].module = pipeline->shaderModuleVS;
	pipelineShaderStageCreateInfos[0].pName = "main";
	pipelineShaderStageCreateInfos[0].pSpecializationInfo = VK_NULL_HANDLE;
	// fragment shader
	pipelineShaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageCreateInfos[1].pNext = VK_NULL_HANDLE;
	pipelineShaderStageCreateInfos[1].flags = 0;
	pipelineShaderStageCreateInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	pipelineShaderStageCreateInfos[1].module = pipeline->shaderModuleFS;
	pipelineShaderStageCreateInfos[1].pName = "main";
	pipelineShaderStageCreateInfos[1].pSpecializationInfo = VK_NULL_HANDLE;

	// VkPipelineVertexInputStateCreateInfo
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineVertexInputStateCreateInfo.pNext = VK_NULL_HANDLE;
	pipelineVertexInputStateCreateInfo.flags = 0;
	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = vertexInputBindingDescriptionCount;
	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions;
	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputAttributeDescriptionCount;
	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions;

	// VkPipelineInputAssemblyStateCreateInfo
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
	pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineInputAssemblyStateCreateInfo.pNext = VK_NULL_HANDLE;
	pipelineInputAssemblyStateCreateInfo.flags = 0;
	pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	// VkPipelineTessellationStateCreateInfo
	VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo{};
	pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	pipelineTessellationStateCreateInfo.pNext = VK_NULL_HANDLE;
	pipelineTessellationStateCreateInfo.flags = 0;
	pipelineTessellationStateCreateInfo.patchControlPoints = 3;

	// VkViewport - viewport
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = 0.0f;
	viewport.height = 0.0f;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// VkRect2D - scissor
	VkRect2D scissor{};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = 0;
	scissor.extent.height = 0;

	// VkPipelineViewportStateCreateInfo
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
	pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipelineViewportStateCreateInfo.pNext = VK_NULL_HANDLE;
	pipelineViewportStateCreateInfo.flags = 0;
	pipelineViewportStateCreateInfo.viewportCount = 1;
	pipelineViewportStateCreateInfo.pViewports = &viewport;
	pipelineViewportStateCreateInfo.scissorCount = 1;
	pipelineViewportStateCreateInfo.pScissors = &scissor;

	// VkPipelineRasterizationStateCreateInfo
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
	pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineRasterizationStateCreateInfo.pNext = VK_NULL_HANDLE;
	pipelineRasterizationStateCreateInfo.flags = 0;
	pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
	pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
	pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

	// VkPipelineMultisampleStateCreateInfo
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
	pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineMultisampleStateCreateInfo.pNext = VK_NULL_HANDLE;
	pipelineMultisampleStateCreateInfo.flags = 0;
	pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	pipelineMultisampleStateCreateInfo.minSampleShading = 1.0f;
	pipelineMultisampleStateCreateInfo.pSampleMask = VK_NULL_HANDLE;
	pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

	// VkPipelineDepthStencilStateCreateInfo
	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
	pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipelineDepthStencilStateCreateInfo.pNext = VK_NULL_HANDLE;
	pipelineDepthStencilStateCreateInfo.flags = 0;
	pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
	pipelineDepthStencilStateCreateInfo.front.failOp = VK_STENCIL_OP_KEEP;
	pipelineDepthStencilStateCreateInfo.front.passOp = VK_STENCIL_OP_KEEP;
	pipelineDepthStencilStateCreateInfo.front.depthFailOp = VK_STENCIL_OP_KEEP;
	pipelineDepthStencilStateCreateInfo.front.compareOp = VK_COMPARE_OP_NEVER;
	pipelineDepthStencilStateCreateInfo.front.compareMask = 0;
	pipelineDepthStencilStateCreateInfo.front.writeMask = 0;
	pipelineDepthStencilStateCreateInfo.front.reference = 0;
	pipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
	pipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
	pipelineDepthStencilStateCreateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
	pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_NEVER;
	pipelineDepthStencilStateCreateInfo.back.compareMask = 0;
	pipelineDepthStencilStateCreateInfo.back.writeMask = 0;
	pipelineDepthStencilStateCreateInfo.back.reference = 0;
	pipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f;
	pipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f;

	// VkPipelineColorBlendStateCreateInfo
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
	pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineColorBlendStateCreateInfo.pNext = VK_NULL_HANDLE;
	pipelineColorBlendStateCreateInfo.flags = 0;
	pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	pipelineColorBlendStateCreateInfo.attachmentCount = pipelineColorBlendAttachmentStateCount;
	pipelineColorBlendStateCreateInfo.pAttachments = pipelineColorBlendAttachmentStates;
	pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	// VkDynamicState - dynamicStates
	std::array<VkDynamicState, 3> dynamicStates = { VK_DYNAMIC_STATE_LINE_WIDTH, VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	// VkPipelineDynamicStateCreateInfo
	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
	pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipelineDynamicStateCreateInfo.pNext = VK_NULL_HANDLE;
	pipelineDynamicStateCreateInfo.flags = 0;
	pipelineDynamicStateCreateInfo.dynamicStateCount = (uint32_t)dynamicStates.size();
	pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

	// VkPipelineLayoutCreateInfo
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pNext = VK_NULL_HANDLE;
	descriptorSetLayoutCreateInfo.flags = 0;
	descriptorSetLayoutCreateInfo.bindingCount = descriptorSetLayoutBindingCount;
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings;
	VKT_CHECK(vkCreateDescriptorSetLayout(device.device, &descriptorSetLayoutCreateInfo, VK_NULL_HANDLE, &pipeline->descriptorSetLayout));
	assert(pipeline->descriptorSetLayout);

	// VkPipelineLayoutCreateInfo
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = VK_NULL_HANDLE;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &pipeline->descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = VK_NULL_HANDLE;
	VKT_CHECK(vkCreatePipelineLayout(device.device, &pipelineLayoutCreateInfo, VK_NULL_HANDLE, &pipeline->pipelineLayout));
	assert(pipeline->pipelineLayout);

	// VkGraphicsPipelineCreateInfo
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.pNext = VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.flags = 0;
	graphicsPipelineCreateInfo.stageCount = (uint32_t)pipelineShaderStageCreateInfos.size();
	graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfos.data();
	graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pTessellationState = &pipelineTessellationStateCreateInfo;
	graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
	graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
	graphicsPipelineCreateInfo.layout = pipeline->pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = renderPass;
	graphicsPipelineCreateInfo.subpass = subpass;
	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.basePipelineIndex = 0;
	VKT_CHECK(vkCreateGraphicsPipelines(device.device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, VK_NULL_HANDLE, &pipeline->pipeline));
	assert(pipeline->pipeline);

	// get descriptor type counts
	std::map<VkDescriptorType, uint32_t> descriptorTypeCounts{};
	for (uint32_t i = 0; i < descriptorSetLayoutBindingCount; i++)
		descriptorTypeCounts[descriptorSetLayoutBindings[i].descriptorType]++;

	// fill descriptor pool sizes
	std::vector<VkDescriptorPoolSize> descriptorPoolSizes{};
	for (const auto& descriptorTypeCount : descriptorTypeCounts)
		descriptorPoolSizes.push_back({ descriptorTypeCount.first, descriptorTypeCount.second });

	// VkDescriptorPoolCreateInfo
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.pNext = VK_NULL_HANDLE;
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolCreateInfo.maxSets = 1;
	descriptorPoolCreateInfo.poolSizeCount = (uint32_t)descriptorPoolSizes.size();
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
	vkCreateDescriptorPool(device.device, &descriptorPoolCreateInfo, VK_NULL_HANDLE, &pipeline->descriptorPool);
	assert(pipeline->descriptorPool);

	// VkDescriptorSetAllocateInfo
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = VK_NULL_HANDLE;
	descriptorSetAllocateInfo.descriptorPool = pipeline->descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &pipeline->descriptorSetLayout;
	VKT_CHECK(vkAllocateDescriptorSets(device.device, &descriptorSetAllocateInfo, &pipeline->descriptorSet));
	assert(pipeline->descriptorSet);
}

// vulkanPipelineBindImage
void vulkanPipelineBindImage(
	VulkanDevice&   device,
	VulkanPipeline& pipeline,
	VulkanImage&    image,
	VulkanSampler&  sampler,
	uint32_t        binding)
{
	// VkDescriptorImageInfo
	VkDescriptorImageInfo descriptorImageInfo{};
	descriptorImageInfo.sampler = sampler.sampler;
	descriptorImageInfo.imageView = image.imageView;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	// VkWriteDescriptorSet
	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = VK_NULL_HANDLE;
	writeDescriptorSet.dstSet = pipeline.descriptorSet;
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSet.pImageInfo = &descriptorImageInfo;
	writeDescriptorSet.pBufferInfo = VK_NULL_HANDLE;
	writeDescriptorSet.pTexelBufferView = VK_NULL_HANDLE;
	vkUpdateDescriptorSets(device.device, 1, &writeDescriptorSet, 0, VK_NULL_HANDLE);
}

// vulkanPipelineBindBufferUniform
void vulkanPipelineBindBufferUniform(
	VulkanDevice&   device,
	VulkanPipeline& pipeline,
	VulkanBuffer&   buffer,
	uint32_t        binding)
{
	// VkDescriptorImageInfo
	VkDescriptorBufferInfo descriptorBufferInfo{};
	descriptorBufferInfo.buffer = buffer.buffer;
	descriptorBufferInfo.offset = 0;
	descriptorBufferInfo.range = VK_WHOLE_SIZE;

	// VkWriteDescriptorSet
	VkWriteDescriptorSet writeDescriptorSet{};
	// VkWriteDescriptorSet - uniform buffer
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.pNext = VK_NULL_HANDLE;
	writeDescriptorSet.dstSet = pipeline.descriptorSet;
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.pImageInfo = VK_NULL_HANDLE;
	writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
	writeDescriptorSet.pTexelBufferView = VK_NULL_HANDLE;
	vkUpdateDescriptorSets(device.device, 1, &writeDescriptorSet, 0, VK_NULL_HANDLE);
}

// vulkanPipelineDestroy
void vulkanPipelineDestroy(
	VulkanDevice&   device,
	VulkanPipeline& pipeline)
{
	// destroy handles
	vkFreeDescriptorSets(device.device, pipeline.descriptorPool, 1, &pipeline.descriptorSet);
	vkDestroyDescriptorPool(device.device, pipeline.descriptorPool, VK_NULL_HANDLE);
	vkDestroyPipeline(device.device, pipeline.pipeline, VK_NULL_HANDLE);
	vkDestroyPipelineLayout(device.device, pipeline.pipelineLayout, VK_NULL_HANDLE);
	vkDestroyDescriptorSetLayout(device.device, pipeline.descriptorSetLayout, VK_NULL_HANDLE);
	vkDestroyShaderModule(device.device, pipeline.shaderModuleFS, VK_NULL_HANDLE);
	vkDestroyShaderModule(device.device, pipeline.shaderModuleVS, VK_NULL_HANDLE);
	// clear handles
	pipeline.descriptorSet = VK_NULL_HANDLE;
	pipeline.descriptorPool = VK_NULL_HANDLE;
	pipeline.pipeline = VK_NULL_HANDLE;
	pipeline.pipelineLayout = VK_NULL_HANDLE;
	pipeline.descriptorSetLayout = VK_NULL_HANDLE;
	pipeline.shaderModuleFS = VK_NULL_HANDLE;
	pipeline.shaderModuleVS = VK_NULL_HANDLE;
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
	VulkanDevice&        device,
	VulkanCommandBuffer& commandBuffer,
	VulkanSemaphore*     waitSemaphore,
	VulkanSemaphore*     signalSemaphore)
{
	// VkSubmitInfo
	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer.commandBuffer;
	// setup wait semaphore
	if (waitSemaphore != nullptr) {
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitSemaphore->semaphore;
	}
	// setup signal semaphore
	if (signalSemaphore != nullptr) {
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore->semaphore;
	}
	VKT_CHECK(vkQueueSubmit(device.queueGraphics, 1, &submitInfo, VK_NULL_HANDLE));
}
