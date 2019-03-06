#pragma once

#include "vulkan_context.hpp"
#include "vulkan_scene.hpp"

// VulkanRenderer
class VulkanRenderer {
protected:
	// base handles
	VulkanContext& context;
	VulkanSurface& surface;
public:
	// constructor and destructor
	VulkanRenderer(
		VulkanContext& context,
		VulkanSurface& surface) :
		context(context),
		surface(surface) {};
	virtual ~VulkanRenderer() {}

	// reinitialize
	virtual void reinitialize() = 0;

	// getters
	virtual float getViewHeight() = 0;
	virtual float getViewWidth() = 0;
	virtual float getViewAspect() = 0;

	// draw functions
	virtual void drawScene(VulkanScene* scene) = 0;
};

// VulkanRenderer_default
class VulkanRenderer_default : public VulkanRenderer {
protected:
	// swapchain
	VulkanSwapchain swapchain{};
protected:
	// frame count
	uint32_t framesCount{};
	// present depth-stencil attachments
	std::vector<VkImageView>   colorAttachmentImageViews{};
	// present depth-stencil attachments
	std::vector<VkImage>       depthStencilAttachmentImages{};
	std::vector<VkImageView>   depthStencilAttachmentImageViews{};
	std::vector<VmaAllocation> depthStencilAttachmentAllocations{};
	// frame buffers and command buffers
	std::vector<VkFramebuffer> framebuffers{};
	// render pass
	VkRenderPass renderPass{};
protected:
	// command buffers
	std::vector<VulkanCommandBuffer> commandBuffers{};
	// render and present semaphores
	std::vector<VulkanSemaphore> renderSemaphores{};
	std::vector<VulkanSemaphore> presentSemaphores{};
protected:
	// shaders
	VulkanShader shader_obj{};
	VulkanShader shader_debug{};
	VulkanShader shader_shadow{};
	VulkanShader shader_gui{};
	// objects pipelines
	VulkanPipeline pipeline_obj[VK_PRIMITIVE_TOPOLOGY_RANGE_SIZE]{};
	VulkanPipeline pipeline_obj_wf[VK_PRIMITIVE_TOPOLOGY_RANGE_SIZE]{};
	VulkanPipeline pipeline_debug[VK_PRIMITIVE_TOPOLOGY_RANGE_SIZE]{};
	VulkanPipeline pipeline_gui[VK_PRIMITIVE_TOPOLOGY_RANGE_SIZE]{};
protected:
	// create functions
	void createSwapchain();
	void createImages();
	void createRenderPass();
	void createFramebuffers();
	void createCommandBuffers();
	void createSemaphores();
	void createShaders();
	void createPipelines();

	// destroy functions
	void destroySwapchain();
	void destroyImages();
	void destroyRenderPass();
	void destroyFramebuffers();
	void destroyCommandBuffers();
	void destroySemaphores();
	void destroyShaders();
	void destroyPipelines();
public:
	// constructor and destructor
	VulkanRenderer_default(VulkanContext& context, VulkanSurface& surface);
	virtual ~VulkanRenderer_default();

	// reinitialize
	void reinitialize() override;

	// getters
	float getViewHeight() override;
	float getViewWidth() override;
	float getViewAspect() override;

	// draw functions
	void drawScene(VulkanScene* scene) override;
};