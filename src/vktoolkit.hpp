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
	VkQueueFamilyProperties          queueFamilyPropertiesGraphics;
	VkQueueFamilyProperties          queueFamilyPropertiesCompute;
	VkQueueFamilyProperties          queueFamilyPropertiesTransfer;
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

typedef struct VulkanSampler {
	VkSampler sampler;
	VkBool32  anisotropyEnable;
} VulkanSampler;

typedef struct VulkanImage {
	VmaAllocation              allocation;
	VmaAllocationInfo          allocationInfo;
	VkImage                    image;
	VkImageView                imageView;
	VkImageType                imageType;
	VkFormat                   format;
	uint32_t                   width;
	uint32_t                   height;
	uint32_t                   depth;
	uint32_t                   mipLevels;
	std::vector<VkAccessFlags> accessFlags;
	std::vector<VkImageLayout> imageLayouts;
} VulkanImage;

typedef struct VulkanBuffer {
	VmaAllocation     allocation;
	VmaAllocationInfo allocationInfo;
	VkBuffer          buffer;
	VkDeviceSize      size;
} VulkanBuffer;

typedef struct VulkanSemaphore {
	VkSemaphore semaphore;
} VulkanSemaphore;

typedef struct VulkanCommandBuffer {
	VkCommandBuffer commandBuffer;
} VulkanCommandBuffer;

typedef struct VulkanSwapchain {
	VkSurfaceFormatKHR         surfaceFormat;
	VkPresentModeKHR           presentMode;
	VkSurfaceCapabilitiesKHR   surfaceCapabilities;
	VkSwapchainKHR             swapchain;
	VkRenderPass               renderPass;
	VkImage                    imageDS;
	VmaAllocation              allocationDS;
	VkImageView                imageViewDS;
	std::vector<VkImage>       images;
	std::vector<VkImageView>   imageViews;
	std::vector<VkFramebuffer> framebuffers;
} VulkanSwapchain;

typedef struct VulkanPipeline {
	VkPipeline            pipeline;
	VkShaderModule        shaderModuleVS;
	VkShaderModule        shaderModuleFS;
	VkPipelineLayout      pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool      descriptorPool;
	VkDescriptorSet       descriptorSet;
} VulkanPipeline;

// create/destroy/read/write

void vulkanInstanceCreate(
	std::vector<const char *>& enabledLayerNames,
	std::vector<const char *>& enabledExtensionNames,
	VulkanInstance* instance
);

void vulkanInstanceDestroy(
	VulkanInstance& instance
);

void vulkanDeviceCreate(
	VulkanInstance&            instance,
	VkPhysicalDeviceType       physicalDeviceType,
	VkPhysicalDeviceFeatures&  physicalDeviceFeatures,
	std::vector<const char *>& enabledExtensionNames,
	VulkanDevice*              device
);

void vulkanDeviceDestroy(
	VulkanDevice& device
);

void vulkanSamplerCreate(
	VulkanDevice&        device,
	VkFilter             filter,
	VkSamplerAddressMode samplerAddressMode,
	VkBool32             anisotropyEnable,
	VulkanSampler*       sampler
);

void vulkanSamplerDestroy(
	VulkanDevice&  device,
	VulkanSampler& sampler
);

void vulkanImageCreate(
	VulkanDevice&     device,
	VkFormat          format,
	uint32_t          width,
	uint32_t          height,
	uint32_t          depth,
	VulkanImage*      image
);

void vulkanImageRead(
	VulkanDevice& device,
	VulkanImage&  image,
	uint32_t      mipLevel,
	void*         data
);

void vulkanImageWrite(
	VulkanDevice& device,
	VulkanImage&  image,
	uint32_t      mipLevel,
	const void*   data
);

void vulkanImageCopy(
	VulkanDevice& device,
	VulkanImage&  imageSrc,
	uint32_t      mipLevelSrc,
	VulkanImage&  imageDst,
	uint32_t      mipLevelDst
);

void vulkanImageBuildMipmaps(
	VulkanDevice& device,
	VulkanImage&  image
);

void vulkanImageSetLayout(
	VulkanCommandBuffer& commandBuffer,
	VulkanImage&         image,
	uint32_t             mipLevel,
	VkAccessFlags        accessFlags,
	VkImageLayout        imageLayout
);

void vulkanImageDestroy(
	VulkanDevice& device,
	VulkanImage&  image
);

void vulkanBufferCreate(
	VulkanDevice&      device,
	VkBufferUsageFlags usage,
	VkDeviceSize       size,
	VulkanBuffer*      buffer
);

void vulkanBufferRead(
	VulkanDevice& device,
	VulkanBuffer& buffer,
	VkDeviceSize  offset,
	VkDeviceSize  size,
	void*         data
);

void vulkanBufferWrite(
	VulkanDevice& device,
	VulkanBuffer& buffer,
	VkDeviceSize  offset,
	VkDeviceSize  size,
	const void*   data
);

void vulkanBufferCopy(
	VulkanDevice& device,
	VulkanBuffer& bufferSrc,
	VkDeviceSize  offsetSrc,
	VulkanBuffer& bufferDst,
	VkDeviceSize  offsetDst,
	VkDeviceSize  size
);

void vulkanBufferDestroy(
	VulkanDevice& device,
	VulkanBuffer& buffer
);

void vulkanCommandBufferAllocate(
	VulkanDevice&        device,
	VkCommandBufferLevel commandBufferLevel,
	VulkanCommandBuffer* commandBuffer
);

void vulkanCommandBufferBegin(
	VulkanDevice&             device,
	VulkanCommandBuffer&      commandBuffer,
	VkCommandBufferUsageFlags flags
);

void vulkanCommandBufferEnd(
	VulkanCommandBuffer& commandBuffer
);

void vulkanCommandBufferFree(
	VulkanDevice&        device,
	VulkanCommandBuffer& commandBuffer
);

void vulkanSemaphoreCreate(
	VulkanDevice&    device,
	VulkanSemaphore* semaphore
);

void vulkanSemaphoreDestroy(
	VulkanDevice&    device,
	VulkanSemaphore& semaphore
);

void vulkanSwapchainCreate(
	VulkanDevice&    device,
	VulkanSurface&   surface,
	VulkanSwapchain* swapchain
);

void vulkanSwapchainDestroy(
	VulkanDevice&    device,
	VulkanSwapchain& swapchain
);

void vulkanSwapchainBeginFrame(
	VulkanDevice&    device,
	VulkanSwapchain& swapchain,
	VulkanSemaphore& semaphore,
	uint32_t*        frameIndex
);

void vulkanSwapchainEndFrame(
	VulkanDevice&    device,
	VulkanSwapchain& swapchain,
	VulkanSemaphore& semaphore,
	uint32_t         frameIndex
);

// init utilities

VkDeviceQueueCreateInfo vulkanInitDeviceQueueCreateInfo(
	uint32_t     queueFamilyIndex,
	uint32_t     queueCount,
	const float* pQueuePriorities
);

// get/find utilities

VkSurfaceFormatKHR vulkanGetDefaultSurfaceFormat(
	VulkanDevice&  device,
	VulkanSurface& surface
);

VkPresentModeKHR vulkanGetDefaultSurfacePresentMode(
	VulkanDevice&  device,
	VulkanSurface& surface
);

uint32_t vulkanFindQueueFamilyPropertiesByFlags(
	std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
	VkQueueFlags                          queueFlags
);

// queue utilities

void vulkanQueueSubmit(
	VulkanDevice&        device,
	VulkanCommandBuffer& commandBuffer,
	VulkanSemaphore*     waitSemaphore,
	VulkanSemaphore*     signalSemaphore
);