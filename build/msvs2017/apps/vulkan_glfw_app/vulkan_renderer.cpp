#include "vulkan_renderer.hpp"
#include "vulkan_pipelines.hpp"
#include "vulkan_loaders.hpp"

// VulkanRender::VulkanRender
VulkanRender::VulkanRender(
	GLFWwindow*                window,
	std::vector<const char *>& enabledInstanceLayerNames,
	std::vector<const char *>& enabledInstanceExtensionNames,
	std::vector<const char *>& enabledDeviceExtensionNames,
	VkPhysicalDeviceFeatures&  physicalDeviceFeatures) 
{
	// create vulkan handlers
	vulkanInstanceCreate(enabledInstanceLayerNames, enabledInstanceExtensionNames, &instance);
	glfwCreateWindowSurface(instance.instance, window, NULL, &surface.surface);
	vulkanDeviceCreate(instance, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, physicalDeviceFeatures, enabledDeviceExtensionNames, &device);
	vulkanSwapchainCreate(device, surface, &swapchain);
	vulkanSemaphoreCreate(device, &renderSemaphore);
	vulkanSemaphoreCreate(device, &presentSemaphore);
	vulkanCommandBufferAllocate(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &commandBuffer);

	// create descriptos set layouts
	vulkanDescriptorSetLayoutCreate(device, VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_material), descriptorSetLayoutBindings_material, &descriptorSetLayout_material);
	vulkanDescriptorSetLayoutCreate(device, VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_model), descriptorSetLayoutBindings_model, &descriptorSetLayout_model);
	vulkanDescriptorSetLayoutCreate(device, VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_scene), descriptorSetLayoutBindings_scene, &descriptorSetLayout_scene);

	// list of descriptor set layout
	VkDescriptorSetLayout descriptorSetLayouts[] = {
		descriptorSetLayout_material.descriptorSetLayout,
		descriptorSetLayout_model.descriptorSetLayout,
		descriptorSetLayout_scene.descriptorSetLayout,
	};

	// create pipeline layout object
	vulkanPipelineLayoutCreate(device, VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayouts), descriptorSetLayouts, &pipelineLayout);

	// create shaders and pipelines
	createPipeline_gui(device, pipelineLayout, swapchain.renderPass, 0, &shader_gui, &pipeline_gui);
	createPipeline_line(device, pipelineLayout, swapchain.renderPass, 0, &shader_line, &pipeline_line);
	createPipeline_obj(device, pipelineLayout, swapchain.renderPass, 0, &shader_obj, &pipeline_obj, &pipeline_obj_wf);

	// create image and sampler default
	createImageProcedural(device, 1024, 1024, imageDefault);
	vulkanSamplerCreate(device, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_TRUE, &samplerDefault);
	materialDefault = new VulkanMaterial(device, pipelineLayout, descriptorSetLayout_material);
	materialDefault->setImage(imageDefault, samplerDefault, 0);
}

// VulkanRender::~VulkanRender
VulkanRender::~VulkanRender() 
{
	// destroy image and sampler default
	delete materialDefault;
	vulkanImageDestroy(device, imageDefault);
	vulkanSamplerDestroy(device, samplerDefault);

	// destroy pipelines
	vulkanPipelineDestroy(device, pipeline_obj_wf);
	vulkanPipelineDestroy(device, pipeline_obj);
	vulkanPipelineDestroy(device, pipeline_line);
	vulkanPipelineDestroy(device, pipeline_gui);
	
	// destroy shaders
	vulkanShaderDestroy(device, shader_obj);
	vulkanShaderDestroy(device, shader_line);
	vulkanShaderDestroy(device, shader_gui);

	// destroy pipeline layouts
	vulkanPipelineLayoutDestroy(device, pipelineLayout);

	// destroy shaders
	vulkanDescriptorSetLayoutDestroy(device, descriptorSetLayout_scene);
	vulkanDescriptorSetLayoutDestroy(device, descriptorSetLayout_model);
	vulkanDescriptorSetLayoutDestroy(device, descriptorSetLayout_material);

	// destroy vulkan handlers
	vulkanCommandBufferFree(device, commandBuffer);
	vulkanSemaphoreDestroy(device, presentSemaphore);
	vulkanSemaphoreDestroy(device, renderSemaphore);
	vulkanSwapchainDestroy(device, swapchain);
	vkDestroySurfaceKHR(instance.instance, surface.surface, NULL);
	vulkanDeviceDestroy(device);
	vulkanInstanceDestroy(instance);
}
