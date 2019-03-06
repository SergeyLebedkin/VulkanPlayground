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

#ifndef VKT_VECTOR_DATA_SIZE
#define VKT_VECTOR_DATA_SIZE(vec) (VkDeviceSize)(vec.size()*sizeof(vec[0]))
#endif

typedef struct VulkanInstance {
	VkInstance                    instance;
	VkDebugReportCallbackEXT      debugReportCallback;
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
	VkCommandPool                    commandPoolTrancient;
	VkBuffer                         bufferStaging;
	VmaAllocation                    bufferStagingAllocation;
	VmaAllocationInfo                bufferStagingAllocationInfo;
} VulkanDevice;

typedef struct VulkanSurface {
	VkSurfaceKHR surface;
} VulkanSurface;

typedef struct VulkanSwapchain {
	VkSurfaceFormatKHR         surfaceFormat;
	VkPresentModeKHR           presentMode;
	VkSurfaceCapabilitiesKHR   surfaceCapabilities;
	VkSwapchainKHR             swapchain;
	std::vector<VkImage>       images;
} VulkanSwapchain;

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
	std::vector<VkAccessFlags> accessFlags{};
	std::vector<VkImageLayout> imageLayouts{};
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

typedef struct VulkanShader {
	VkShaderModule shaderModuleVS;
	VkShaderModule shaderModuleFS;
} VulkanShader;

typedef struct VulkanDescriptorSetLayout {
	VkDescriptorSetLayout                     descriptorSetLayout;
	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings{};
	std::vector<VkDescriptorPoolSize>         descriptorPoolSizes{};
} VulkanDescriptorSetLayout;

typedef struct VulkanPipelineLayout {
	VkPipelineLayout pipelineLayout;
} VulkanPipelineLayout;

typedef struct VulkanPipeline {
	VkPipeline          pipeline;
	VkPolygonMode       polygonMode;
	VkPrimitiveTopology primitiveTopology;
} VulkanPipeline;

typedef struct VulkanDescriptorSet {
	VkDescriptorPool descriptorPool;
	VkDescriptorSet  descriptorSet;
} VulkanDescriptorSet;

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
	VulkanDevice& device,
	VkFormat      format,
	uint32_t      width,
	uint32_t      height,
	uint32_t      depth,
	VulkanImage*  image
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

void vulkanShaderCreate(
	VulkanDevice& device,
	const char*   fileNameVS,
	const char*   fileNameFS,
	VulkanShader* shader
);

void vulkanShaderDestroy(
	VulkanDevice& device,
	VulkanShader& shader
);

void vulkanDescriptorSetLayoutCreate(
	VulkanDevice&                      device,
	uint32_t                           descriptorSetLayoutBindingCount,
	const VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[],
	VulkanDescriptorSetLayout*         descriptorSetLayout
);

void vulkanDescriptorSetLayoutDestroy(
	VulkanDevice&              device,
	VulkanDescriptorSetLayout& descriptorSetLayout
);

void vulkanPipelineLayoutCreate(
	VulkanDevice&               device,
	uint32_t                    descriptorSetLayoutCount,
	const VkDescriptorSetLayout descriptorSetLayouts[],
	VulkanPipelineLayout*       pipelineLayout);

void vulkanPipelineLayoutDestroy(
	VulkanDevice&         device,
	VulkanPipelineLayout& pipelineLayout);

void vulkanPipelineCreate(
	VulkanDevice&                             device,
	VulkanShader&                             shader,
	VulkanPipelineLayout&                     pipelineLayout,
	VkRenderPass                              renderPass,
	uint32_t                                  subpass,
	VkPrimitiveTopology                       primitiveTopology,
	VkPolygonMode                             polygonMode,
	uint32_t                                  vertexInputBindingDescriptionCount,
	const VkVertexInputBindingDescription     vertexInputBindingDescriptions[],
	uint32_t                                  vertexInputAttributeDescriptionCount,
	const VkVertexInputAttributeDescription   vertexInputAttributeDescriptions[],
	uint32_t                                  pipelineColorBlendAttachmentStateCount,
	const VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentStates[],
	VulkanPipeline*                           pipeline
);

void vulkanPipelineDestroy(
	VulkanDevice&   device,
	VulkanPipeline& pipeline
);

void vulkanDescriptorSetCreate(
	VulkanDevice&              device,
	VulkanDescriptorSetLayout& descriptorSetLayout,
	VulkanDescriptorSet*       descriptorSet
);

void vulkanDescriptorSetUpdateImage(
	VulkanDevice&        device,
	VulkanDescriptorSet& descriptorSet,
	VulkanImage&         image,
	VulkanSampler&       sampler,
	uint32_t             binding
);

void vulkanDescriptorSetUpdateBufferUniform(
	VulkanDevice&        device,
	VulkanDescriptorSet& descriptorSet,
	VulkanBuffer&        buffer,
	uint32_t             binding
);

void vulkanDescriptorSetDestroy(
	VulkanDevice&        device,
	VulkanDescriptorSet& descriptorSet
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