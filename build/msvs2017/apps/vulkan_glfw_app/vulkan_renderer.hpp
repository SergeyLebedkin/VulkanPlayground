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

	// draw scene
	virtual void drawScene(VulkanScene* scene) = 0;
};

// VulkanRenderer_default
class VulkanRenderer_default : public VulkanRenderer {
protected:
	// swapchain
	VulkanSwapchain swapchain{};
protected:
	// present depth-stencil attachments
	std::vector<VkImage>       colorAttachementImages{};
	std::vector<VkImageView>   colorAttachementImageViews{};
	std::vector<VmaAllocation> colorAttachementAllocations{};
	// present depth-stencil attachments
	std::vector<VkImage>       depthStencilAttachementImages{};
	std::vector<VkImageView>   depthStencilAttachementImageViews{};
	std::vector<VmaAllocation> depthStencilAttachementAllocations{};
	// frame buffers and command buffers
	std::vector<VkFramebuffer>   frameBuffers{};
	std::vector<VkCommandBuffer> commandBuffers{};
	// render and present semaphores
	std::vector<VkSemaphore> renderSemaphores{};
	std::vector<VkSemaphore> presentSemaphores{};
protected:
	// shaders
	VulkanShader shader_obj{};
	VulkanShader shader_debug{};
	VulkanShader shader_shadow{};
	VulkanShader shader_gui{};
	// objects pipelines
	VulkanPipeline pipeline_obj_point{};
	VulkanPipeline pipeline_obj_line{};
	VulkanPipeline pipeline_obj_linestrip{};
	VulkanPipeline pipeline_obj_tri{};
	VulkanPipeline pipeline_obj_tristrip{};
	VulkanPipeline pipeline_obj_trifan{};
	// objects pipelines
	VulkanPipeline pipeline_obj_tri_wf{};
	VulkanPipeline pipeline_obj_tristrip_wf{};
	VulkanPipeline pipeline_obj_trifan_wf{};
	// shadow pipelines
	VulkanPipeline pipeline_shadow_point{};
	VulkanPipeline pipeline_shadow_line{};
	VulkanPipeline pipeline_shadow_linestrip{};
	VulkanPipeline pipeline_shadow_tri{};
	VulkanPipeline pipeline_shadow_tristrip{};
	VulkanPipeline pipeline_shadow_trifan{};
	// debug pipelines
	VulkanPipeline pipeline_debug_point{};
	VulkanPipeline pipeline_debug_line{};
	VulkanPipeline pipeline_debug_linestrip{};
	VulkanPipeline pipeline_debug_tri{};
	VulkanPipeline pipeline_debug_tristrip{};
	VulkanPipeline pipeline_debug_trifan{};
	// GUI pipelines
	VulkanPipeline pipeline_gui_point{};
	VulkanPipeline pipeline_gui_line{};
	VulkanPipeline pipeline_gui_linestrip{};
	VulkanPipeline pipeline_gui_tri{};
	VulkanPipeline pipeline_gui_tristrip{};
	VulkanPipeline pipeline_gui_trifan{};
protected:
	// render pass
	VkRenderPass renderPass{};
protected:
	// create functions
	void createSwapchain();
	void createImages();
	void createFramebuffers();
	void createCommandBuffers();
	void createSemaphores();
	void createPipelines();
	void createRenderPass();

	// destroy functions
	void destroySwapchain();
	void destroyImages();
	void destroyFramebuffers();
	void destroyCommandBuffers();
	void destroySemaphores();
	void destroyPipelines();
	void destroyRenderPass();
public:
	// constructor and destructor
	VulkanRenderer_default(
		VulkanContext& context, 
		VulkanSurface& surface) :
		VulkanRenderer(context, surface) {}
	virtual ~VulkanRenderer_default() {}

	// reinitialize
	void reinitialize() {};

	// draw functions
	void beginFrame() {};
	void drawScene(VulkanScene* scene) {};
	void endFrame() {};
};