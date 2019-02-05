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
	device.physicalDeviceMemoryProperties = {};
	device.physicalDeviceProperties = {};
	device.physicalDeviceFeatures = {};
	device.physicalDevice = VK_NULL_HANDLE;
}

// vulkanBufferCreate
void vulkanBufferCreate(
	VulkanDevice& device,
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VulkanBuffer* buffer)
{
	// check size
	assert(buffer);

	// VkBufferCreateInfo
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = VK_NULL_HANDLE;
	bufferCreateInfo.size = 0;
	bufferCreateInfo.usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;

	// VmaAllocationCreateInfo
	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocCreateInfo.flags = 0;

	// vmaCreateBuffer
	VKT_CHECK(vmaCreateBuffer(device.allocator, &bufferCreateInfo, &allocCreateInfo, &buffer->buffer, &buffer->allocation, VK_NULL_HANDLE));
	assert(buffer->buffer);
	assert(buffer->allocation);

	// VkBufferViewCreateInfo
	VkBufferViewCreateInfo bufferViewCreateInfo{};
	bufferViewCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
	bufferViewCreateInfo.pNext = VK_NULL_HANDLE;
	bufferViewCreateInfo.flags = 0;
	bufferViewCreateInfo.buffer = buffer->buffer;
	bufferViewCreateInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	bufferViewCreateInfo.offset = 0;
	bufferViewCreateInfo.range = VK_WHOLE_SIZE;
	vkCreateBufferView(device.device, &bufferViewCreateInfo, VK_NULL_HANDLE, &buffer->bufferView);
	assert(buffer->bufferView);
}

// vulkanBufferDestroy
void vulkanBufferDestroy(
	VulkanDevice& device,
	VulkanBuffer* buffer)
{
	// check handles
	assert(buffer);
	// check handles
	vkDestroyBufferView(device.device, buffer->bufferView, VK_NULL_HANDLE);
	vmaDestroyBuffer(device.allocator, buffer->buffer, buffer->allocation);
	// clear handles
	buffer->bufferView = VK_NULL_HANDLE;
	buffer->buffer = VK_NULL_HANDLE;
	buffer->allocation = VK_NULL_HANDLE;
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

// vulkanSemaphoreCreate
void vulkanSemaphoreCreate(VulkanDevice& device, VulkanSemaphore* semaphore)
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
void vulkanSemaphoreDestroy(VulkanDevice& device, VulkanSemaphore& semaphore)
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
