#pragma once
#include <vktoolkit.hpp>
#include <GLFW/glfw3.h>

// VulkanRender
class VulkanRender {
public:
	// init vulkan
	VulkanInstance      instance{};
	VulkanSurface       surface{};
	VulkanDevice        device{};
	VulkanSwapchain     swapchain{};
	VulkanSemaphore     renderSemaphore{};
	VulkanSemaphore     presentSemaphore{};
	VulkanCommandBuffer commandBuffer{};

	// pipelines and shaders
	VulkanShader shader_obj{};
	VulkanShader shader_line{};
	VulkanShader shader_default{};
	VulkanPipeline pipeline_obj{};
	VulkanPipeline pipeline_obj_wf{};
	VulkanPipeline pipeline_line{};
	VulkanPipeline pipeline_default{};

	// images and samplers
	VulkanSampler samplerDefault{};
	VulkanImage imageDefault{};
public:
	VulkanRender(
		GLFWwindow*                window,
		std::vector<const char *>& enabledInstanceLayerNames,
		std::vector<const char *>& enabledInstanceExtensionNames,
		std::vector<const char *>& enabledDeviceExtensionNames,
		VkPhysicalDeviceFeatures&  physicalDeviceFeatures);
	virtual ~VulkanRender();
};