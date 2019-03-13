#pragma once

#include "vulkan_context.hpp"
#include "vulkan_scene.hpp"

// VulkanRenderer
class VulkanRenderer;

// renderer callback function type
typedef void(* VulkanRendererCallbackFunc)(VulkanRenderer& renderer, VulkanCommandBuffer& commandBuffer);

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
	virtual uint32_t getViewHeight() = 0;
	virtual uint32_t getViewWidth() = 0;
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
	// swapchain frames and image indexes
	uint32_t frameIndex{};
	uint32_t framesCount{};
protected:
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
	// mesh object vertex shader files
	const char* shaders_mesh_obj_files_vert[VULKAN_MATERIAL_USAGE_RANGE_SIZE]{
		"shaders/mesh_obj_color.vert.spv",
		"shaders/mesh_obj_color_light.vert.spv",
		"shaders/mesh_obj_color_texture.vert.spv",
		"shaders/mesh_obj_color_texture_light.vert.spv",
		"shaders/mesh_obj_color_texture_bump.vert.spv",
		"shaders/mesh_obj_color_texture_pbr.vert.spv"
	};
	// mesh object fragment shader files
	const char* shaders_mesh_obj_files_frag[VULKAN_MATERIAL_USAGE_RANGE_SIZE]{
		"shaders/mesh_obj_color.frag.spv",
		"shaders/mesh_obj_color_light.frag.spv",
		"shaders/mesh_obj_color_texture.frag.spv",
		"shaders/mesh_obj_color_texture_light.frag.spv",
		"shaders/mesh_obj_color_texture_bump.frag.spv",
		"shaders/mesh_obj_color_texture_pbr.frag.spv"
	};
	// mesh object skin vertex shader files
	const char* shaders_mesh_obj_skin_files_vert[VULKAN_MATERIAL_USAGE_RANGE_SIZE]{
		"shaders/mesh_obj_skin_color.vert.spv",
		"shaders/mesh_obj_skin_color_light.vert.spv",
		"shaders/mesh_obj_skin_color_texture.vert.spv",
		"shaders/mesh_obj_skin_color_texture_light.vert.spv",
		"shaders/mesh_obj_skin_color_texture_bump.vert.spv",
		"shaders/mesh_obj_skin_color_texture_pbr.vert.spv"
	};
	// mesh object skin fragment shader files
	const char* shaders_mesh_obj_skin_files_frag[VULKAN_MATERIAL_USAGE_RANGE_SIZE]{
		"shaders/mesh_obj_skin_color.frag.spv",
		"shaders/mesh_obj_skin_color_light.frag.spv",
		"shaders/mesh_obj_skin_color_texture.frag.spv",
		"shaders/mesh_obj_skin_color_texture_light.frag.spv",
		"shaders/mesh_obj_skin_color_texture_bump.frag.spv",
		"shaders/mesh_obj_skin_color_texture_pbr.frag.spv"
	};
	// shaders
	VulkanShader shader_mesh_obj[VULKAN_MATERIAL_USAGE_RANGE_SIZE]{};
	VulkanShader shader_mesh_obj_skin[VULKAN_MATERIAL_USAGE_RANGE_SIZE]{};
	VulkanShader shader_shadow_mesh_obj{};
	VulkanShader shader_shadow_mesh_obj_skin{};
	// objects pipelines
	VulkanPipeline pipeline_mesh_obj[VULKAN_MATERIAL_USAGE_RANGE_SIZE][VK_PRIMITIVE_TOPOLOGY_RANGE_SIZE]{};
	VulkanPipeline pipeline_mesh_obj_wf[VULKAN_MATERIAL_USAGE_RANGE_SIZE][VK_PRIMITIVE_TOPOLOGY_RANGE_SIZE]{};
	VulkanPipeline pipeline_mesh_obj_skin[VULKAN_MATERIAL_USAGE_RANGE_SIZE][VK_PRIMITIVE_TOPOLOGY_RANGE_SIZE]{};
	VulkanPipeline pipeline_mesh_obj_skin_wf[VULKAN_MATERIAL_USAGE_RANGE_SIZE][VK_PRIMITIVE_TOPOLOGY_RANGE_SIZE]{};
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
	uint32_t getViewHeight() override;
	uint32_t getViewWidth() override;
	float getViewAspect() override;

	// draw functions
	void drawScene(VulkanScene* scene) override;
protected:
	// render pass functions
	void beforeRenderPass(VulkanCommandBuffer& commandBuffer, VulkanScene* scene);
	void presentSubPass(VulkanCommandBuffer& commandBuffer, VulkanScene* scene);
	void afterRenderPass(VulkanCommandBuffer& commandBuffer, VulkanScene* scene);
};
