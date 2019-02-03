#pragma once

#include <VmaUsage.h>
#include <vector>

#ifdef _DEBUG
#define VKT_CHECK(func) { VkResult result = func; assert(result == VK_SUCCESS); };
#else
#define VKT_CHECK(func)
#endif

#ifndef VKT_ARRAY_ELEMENTS_COUNT
#define VKT_ARRAY_ELEMENTS_COUNT(arr) (uint32_t)(sizeof(arr)/sizeof(arr[0]))
#endif

typedef struct VulkanInstance {
	VkInstance                    instance;
	std::vector<VkPhysicalDevice> physicalDevices;
} VulkanInstance;

typedef struct VulkanDevice {
	VkPhysicalDevice                 physicalDevice;
	VkPhysicalDeviceFeatures         physicalDeviceFeatures;
	VkPhysicalDeviceProperties       physicalDeviceProperties;
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	uint32_t                         queueFamilyIndexGraphics;
	uint32_t                         queueFamilyIndexCompute;
	uint32_t                         queueFamilyIndexTransfer;
	VkDevice                         device;
	VkQueue                          queueGraphics;
	VkQueue                          queueCompute;
	VkQueue                          queueTransfer;
	VmaAllocator                     allocator;
	VkCommandPool                    commandPool;
} VulkanDevice;

typedef struct VulkanSurface {
	VkSurfaceKHR surface;
} VulkanSurface;

typedef struct VulkanImage {
	VmaAllocation allocation;
	VkImage       image;
	VkSampler     sampler;
	VkImageView   imageView;
} VulkanImage;

typedef struct VulkanBuffer {
	VmaAllocation allocation;
	VkBuffer      buffer;
	VkBufferView  bufferView;
} VulkanBuffer;

typedef struct VulkanSwapchain {
	VkSurfaceFormatKHR         surfaceFormat;
	VkPresentModeKHR           presentMode;
	VkSurfaceCapabilitiesKHR   surfaceCapabilities;
	VkSwapchainKHR             swapchain;
	VkImage                    imageDS;
	VmaAllocation              allocationDS;
	VkImageView                imageViewDS;
	std::vector<VkImage>       images;
	std::vector<VkImageView>   imageViews;
	std::vector<VkFramebuffer> framebuffers;
} VulkanSwapchain;

typedef struct VulkanRenderPass {
	VkRenderPass renderPass;
} VulkanRenderPass;

typedef struct VulkanPipeline {
	VkPipeline            pipeline;
	VkShaderModule        shaderModuleVS;
	VkShaderModule        shaderModuleFS;
	VkPipelineLayout      pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool      descriptorPool;
	VkDescriptorSet       descriptorSet;
} VulkanPipeline;

// create and destroy

void vulkanInstanceCreate(
	std::vector<const char *>& enabledLayerNames,
	std::vector<const char *>& enabledExtensionNames,
	VulkanInstance* instance
);

void vulkanInstanceDestroy(
	VulkanInstance* instance
);

void vulkanDeviceCreate(
	VulkanInstance*            instance,
	VkPhysicalDeviceType       physicalDeviceType,
	VkPhysicalDeviceFeatures&  physicalDeviceFeatures,
	std::vector<const char *>& enabledExtensionNames,
	VulkanDevice*              device
);

void vulkanDeviceDestroy(
	VulkanDevice* device
);

void vulkanSwapchainCreate(
	VulkanDevice*    device,
	VulkanSurface*   surface,
	VulkanSwapchain* swapchain
);

void vulkanSwapchainDestroy(
	VulkanDevice*    device,
	VulkanSwapchain* swapchain
);

// init utils

VkDeviceQueueCreateInfo vulkanInitDeviceQueueCreateInfo(
	uint32_t     queueFamilyIndex,
	uint32_t     queueCount,
	const float* pQueuePriorities
);

// get/find utils

VkSurfaceFormatKHR vulkanGetDefaultSurfaceFormat(
	VulkanDevice*  device,
	VulkanSurface* surface
);

VkPresentModeKHR vulkanGetDefaultSurfacePresentMode(
	VulkanDevice*  device,
	VulkanSurface* surface
);

uint32_t vulkanFindQueueFamilyPropertiesByFlags(
	std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
	VkQueueFlags                          queueFlags
);