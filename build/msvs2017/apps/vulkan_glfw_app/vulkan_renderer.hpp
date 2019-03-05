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

	// draw functions
	virtual void beginFrame() = 0;
	virtual void drawScene(VulkanScene* scene) = 0;
	virtual void endFrame() = 0;
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
	std::vector<VkImage>       colorAttachementImages{};
	std::vector<VkImageView>   colorAttachementImageViews{};
	// present depth-stencil attachments
	std::vector<VkImage>       depthStencilAttachementImages{};
	std::vector<VkImageView>   depthStencilAttachementImageViews{};
	std::vector<VmaAllocation> depthStencilAttachementAllocations{};
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
	VulkanPipeline pipeline_obj_point{};
	VulkanPipeline pipeline_obj_line{};
	VulkanPipeline pipeline_obj_linestrip{};
	VulkanPipeline pipeline_obj_tri{};
	VulkanPipeline pipeline_obj_tristrip{};
	VulkanPipeline pipeline_obj_trifan{};
	// objects pipelines (wireframe)
	VulkanPipeline pipeline_obj_tri_wf{};
	VulkanPipeline pipeline_obj_tristrip_wf{};
	VulkanPipeline pipeline_obj_trifan_wf{};
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
	// create functions
	void createSwapchain();
	void createImages();
	void createFramebuffers();
	void createRenderPass();
	void createCommandBuffers();
	void createSemaphores();
	void createShaders();
	void createPipelines();

	// destroy functions
	void destroySwapchain();
	void destroyImages();
	void destroyFramebuffers();
	void destroyRenderPass();
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

	// draw functions
	void beginFrame() override {};
	void drawScene(VulkanScene* scene) override {};
	void endFrame() override {};
};