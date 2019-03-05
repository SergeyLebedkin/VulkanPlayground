#include "vulkan_renderer.hpp"
#include "vulkan_pipelines.hpp"
#include "vulkan_loaders.hpp"

// VulkanRenderer_default::VulkanRenderer_default
VulkanRenderer_default::VulkanRenderer_default(
	VulkanContext& context,
	VulkanSurface& surface) :
	VulkanRenderer(context, surface)
{
	// create swapchain
	createSwapchain();
	createImages();
	createFramebuffers();
	createRenderPass();
	createCommandBuffers();
	createSemaphores();
	createShaders();
	createPipelines();
}

// VulkanRenderer_default::~VulkanRenderer_default
VulkanRenderer_default::~VulkanRenderer_default() 
{
	// destroy handles
	destroyPipelines();
	destroyShaders();
	destroySemaphores();
	destroyCommandBuffers();
	destroyRenderPass();
	destroyFramebuffers();
	destroyImages();
	destroySwapchain();
}

// VulkanRenderer_default::createSwapchain
void VulkanRenderer_default::createSwapchain() {
	// create swapchain
	vulkanSwapchainCreate(context.device, surface, &swapchain);
}

// VulkanRenderer_default::createImages
void VulkanRenderer_default::createImages() {}

// VulkanRenderer_default::createFramebuffers
void VulkanRenderer_default::createFramebuffers() {}

// VulkanRenderer_default::createRenderPass
void VulkanRenderer_default::createRenderPass() {}

// VulkanRenderer_default::createCommandBuffers
void VulkanRenderer_default::createCommandBuffers() {
	// create command buffers
	commandBuffers.resize(framesCount);
	for (uint32_t frameIndex = 0; frameIndex < framesCount; frameIndex++)
		vulkanCommandBufferAllocate(context.device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &commandBuffers[frameIndex]);
}

// VulkanRenderer_default::createSemaphores
void VulkanRenderer_default::createSemaphores() {
	// create render semaphores
	renderSemaphores.resize(framesCount);
	for (uint32_t frameIndex = 0; frameIndex < framesCount; frameIndex++)
		vulkanSemaphoreCreate(context.device, &renderSemaphores[frameIndex]);
	// create present semaphores
	presentSemaphores.resize(framesCount);
	for (uint32_t frameIndex = 0; frameIndex < framesCount; frameIndex++)
		vulkanSemaphoreCreate(context.device, &presentSemaphores[frameIndex]);
}

// VulkanRenderer_default::createShaders
void VulkanRenderer_default::createShaders() {
	// cretae all shaders
	vulkanShaderCreate(context.device, "shaders/obj.vert.spv", "shaders/obj.frag.spv", &shader_obj);
	vulkanShaderCreate(context.device, "shaders/debug.vert.spv", "shaders/debug.frag.spv", &shader_debug);
	vulkanShaderCreate(context.device, "shaders/shadow.vert.spv", "shaders/shadow.frag.spv", &shader_shadow);
	vulkanShaderCreate(context.device, "shaders/gui.vert.spv", "shaders/obj.frag.spv", &shader_gui);
}

// VulkanRenderer_default::createPipelines
void VulkanRenderer_default::createPipelines() {
	// create objects pipelines
	vulkanPipelineCreate(context.device, shader_obj, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_POINT_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_T2_N3), vertexBindingDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_T2_N3), vertexInputAttributeDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_obj_point);
	vulkanPipelineCreate(context.device, shader_obj, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_T2_N3), vertexBindingDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_T2_N3), vertexInputAttributeDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_obj_line);
	vulkanPipelineCreate(context.device, shader_obj, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_T2_N3), vertexBindingDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_T2_N3), vertexInputAttributeDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_obj_linestrip);
	vulkanPipelineCreate(context.device, shader_obj, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_T2_N3), vertexBindingDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_T2_N3), vertexInputAttributeDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_obj_tri);
	vulkanPipelineCreate(context.device, shader_obj, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_T2_N3), vertexBindingDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_T2_N3), vertexInputAttributeDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_obj_tristrip);
	vulkanPipelineCreate(context.device, shader_obj, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_T2_N3), vertexBindingDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_T2_N3), vertexInputAttributeDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_obj_trifan);

	// objects pipelines (wireframe)
	vulkanPipelineCreate(context.device, shader_obj, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_LINE,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_T2_N3), vertexBindingDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_T2_N3), vertexInputAttributeDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_obj_tri_wf);
	vulkanPipelineCreate(context.device, shader_obj, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, VK_POLYGON_MODE_LINE,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_T2_N3), vertexBindingDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_T2_N3), vertexInputAttributeDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_obj_tristrip_wf);
	vulkanPipelineCreate(context.device, shader_obj, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN, VK_POLYGON_MODE_LINE,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_T2_N3), vertexBindingDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_T2_N3), vertexInputAttributeDescriptions_P3_T2_N3,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_obj_trifan_wf);

	// create debug pipelines
	vulkanPipelineCreate(context.device, shader_debug, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_POINT_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_C4), vertexBindingDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_C4), vertexInputAttributeDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_debug_point);
	vulkanPipelineCreate(context.device, shader_debug, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_C4), vertexBindingDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_C4), vertexInputAttributeDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_debug_line);
	vulkanPipelineCreate(context.device, shader_debug, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_C4), vertexBindingDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_C4), vertexInputAttributeDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_debug_linestrip);
	vulkanPipelineCreate(context.device, shader_debug, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_C4), vertexBindingDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_C4), vertexInputAttributeDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_debug_tri);
	vulkanPipelineCreate(context.device, shader_debug, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_C4), vertexBindingDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_C4), vertexInputAttributeDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_debug_tristrip);
	vulkanPipelineCreate(context.device, shader_debug, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P3_C4), vertexBindingDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P3_C4), vertexInputAttributeDescriptions_P3_C4,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_debug_trifan);

	// create gui pipelines
	vulkanPipelineCreate(context.device, shader_gui, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_POINT_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P4_C4_T2), vertexBindingDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P4_C4_T2), vertexInputAttributeDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_gui_point);
	vulkanPipelineCreate(context.device, shader_gui, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P4_C4_T2), vertexBindingDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P4_C4_T2), vertexInputAttributeDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_gui_line);
	vulkanPipelineCreate(context.device, shader_gui, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P4_C4_T2), vertexBindingDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P4_C4_T2), vertexInputAttributeDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_gui_linestrip);
	vulkanPipelineCreate(context.device, shader_gui, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P4_C4_T2), vertexBindingDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P4_C4_T2), vertexInputAttributeDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_gui_tri);
	vulkanPipelineCreate(context.device, shader_gui, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P4_C4_T2), vertexBindingDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P4_C4_T2), vertexInputAttributeDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_gui_tristrip);
	vulkanPipelineCreate(context.device, shader_gui, context.pipelineLayout, renderPass, 0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN, VK_POLYGON_MODE_FILL,
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P4_C4_T2), vertexBindingDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P4_C4_T2), vertexInputAttributeDescriptions_P4_C4_T2,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
		&pipeline_gui_trifan);
}

// VulkanRenderer_default::destroySwapchain
void VulkanRenderer_default::destroySwapchain() {
	// destroy swapchain
	vulkanSwapchainDestroy(context.device, swapchain);
}

// VulkanRenderer_default::destroyImages
void VulkanRenderer_default::destroyImages() {
	// destroy images
	for (uint32_t i = 0; i < colorAttachementImageViews.size(); i++) {
		// destroy color attachement image views
		vkDestroyImageView(context.device.device, colorAttachementImageViews[i], VK_NULL_HANDLE);
		colorAttachementImageViews[i] = VK_NULL_HANDLE;
		// destroy depth-stencil attachement image views
		vkDestroyImageView(context.device.device, depthStencilAttachementImageViews[i], VK_NULL_HANDLE);
		depthStencilAttachementImageViews[i] = VK_NULL_HANDLE;
		// destroy depth-stencil attachement images
		vmaDestroyImage(context.device.allocator, depthStencilAttachementImages[i], depthStencilAttachementAllocations[i]);
		depthStencilAttachementImages[i] = VK_NULL_HANDLE;
		depthStencilAttachementAllocations[i] = {};
	}
}

// VulkanRenderer_default::destroyFramebuffers
void VulkanRenderer_default::destroyFramebuffers() {
	// destroy framebuffers
	for (auto& framebuffer : framebuffers) {
		vkDestroyFramebuffer(context.device.device, framebuffer, VK_NULL_HANDLE);
		framebuffer = VK_NULL_HANDLE;
	}
}

// VulkanRenderer_default::destroyRenderPass
void VulkanRenderer_default::destroyRenderPass() {
	// destroy render pass
	vkDestroyRenderPass(context.device.device, renderPass, VK_NULL_HANDLE);
	renderPass = VK_NULL_HANDLE;
}

// VulkanRenderer_default::destroyCommandBuffers
void VulkanRenderer_default::destroyCommandBuffers() {
	// destroy command buffers
	for (auto& commandBuffer : commandBuffers)
		vulkanCommandBufferFree(context.device, commandBuffer);
}

// VulkanRenderer_default::destroySemaphores
void VulkanRenderer_default::destroySemaphores() {
	// destroy present semaphores
	for (auto& semaphore : presentSemaphores)
		vulkanSemaphoreDestroy(context.device, semaphore);
	// destroy render semaphores
	for (auto& semaphore : renderSemaphores)
		vulkanSemaphoreDestroy(context.device, semaphore);
}

// VulkanRenderer_default::destroyShaders
void VulkanRenderer_default::destroyShaders() {
	// destroy all shaders
	vulkanShaderDestroy(context.device, shader_gui);
	vulkanShaderDestroy(context.device, shader_shadow);
	vulkanShaderDestroy(context.device, shader_debug);
	vulkanShaderDestroy(context.device, shader_obj);
}

// VulkanRenderer_default::destroyPipelines
void VulkanRenderer_default::destroyPipelines() {
	// destroy gui pipelines
	vulkanPipelineDestroy(context.device, pipeline_gui_trifan);
	vulkanPipelineDestroy(context.device, pipeline_gui_tristrip);
	vulkanPipelineDestroy(context.device, pipeline_gui_tri);
	vulkanPipelineDestroy(context.device, pipeline_gui_linestrip);
	vulkanPipelineDestroy(context.device, pipeline_gui_line);
	vulkanPipelineDestroy(context.device, pipeline_gui_point);
	// destroy debug pipelines
	vulkanPipelineDestroy(context.device, pipeline_debug_trifan);
	vulkanPipelineDestroy(context.device, pipeline_debug_tristrip);
	vulkanPipelineDestroy(context.device, pipeline_debug_tri);
	vulkanPipelineDestroy(context.device, pipeline_debug_linestrip);
	vulkanPipelineDestroy(context.device, pipeline_debug_line);
	vulkanPipelineDestroy(context.device, pipeline_debug_point);
	// destroy objects pipelines (wireframe)
	vulkanPipelineDestroy(context.device, pipeline_obj_trifan_wf);
	vulkanPipelineDestroy(context.device, pipeline_obj_tristrip_wf);
	vulkanPipelineDestroy(context.device, pipeline_obj_tri_wf);
	// destroy objects pipelines
	vulkanPipelineDestroy(context.device, pipeline_obj_trifan);
	vulkanPipelineDestroy(context.device, pipeline_obj_tristrip);
	vulkanPipelineDestroy(context.device, pipeline_obj_tri);
	vulkanPipelineDestroy(context.device, pipeline_obj_linestrip);
	vulkanPipelineDestroy(context.device, pipeline_obj_line);
	vulkanPipelineDestroy(context.device, pipeline_obj_point);
}

// VulkanRenderer_default::reinitialize
void VulkanRenderer_default::reinitialize() {
	// destroy all related handles
	destroyPipelines();
	destroyRenderPass();
	destroyFramebuffers();
	destroyImages();
	destroySwapchain();
	// create all related handles
	createSwapchain();
	createImages();
	createFramebuffers();
	createRenderPass();
	createPipelines();
}

// VulkanRenderer_default::beginFrame
void VulkanRenderer_default::beginFrame() {};

// VulkanRenderer_default::drawScene
void VulkanRenderer_default::drawScene(VulkanScene* scene) {};

// VulkanRenderer_default::endFrame
void VulkanRenderer_default::endFrame() {};
