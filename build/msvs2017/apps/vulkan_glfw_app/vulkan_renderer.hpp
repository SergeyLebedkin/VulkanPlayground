#pragma once
#include <vktoolkit.hpp>
#include <GLFW/glfw3.h>
#include "vulkan_material.hpp"

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

	// descriptor set layouts
	VulkanDescriptorSetLayout descriptorSetLayout_material{}; // set 0
	VulkanDescriptorSetLayout descriptorSetLayout_model{};    // set 1
	VulkanDescriptorSetLayout descriptorSetLayout_scene{};    // set 2

	// pipeline layout
	VulkanPipelineLayout pipelineLayout{};

	// shaders
	VulkanShader shader_gui{};
	VulkanShader shader_line{};
	VulkanShader shader_obj{};

	// pipelines
	VulkanPipeline pipeline_gui{};
	VulkanPipeline pipeline_line{};
	VulkanPipeline pipeline_obj{};
	VulkanPipeline pipeline_obj_wf{};

	// images and samplers
	VulkanMaterial* materialDefault{};
	VulkanSampler   samplerDefault{};
	VulkanImage     imageDefault{};
public:
	VulkanRender(
		GLFWwindow*                window,
		std::vector<const char *>& enabledInstanceLayerNames,
		std::vector<const char *>& enabledInstanceExtensionNames,
		std::vector<const char *>& enabledDeviceExtensionNames,
		VkPhysicalDeviceFeatures&  physicalDeviceFeatures);
	virtual ~VulkanRender();
};