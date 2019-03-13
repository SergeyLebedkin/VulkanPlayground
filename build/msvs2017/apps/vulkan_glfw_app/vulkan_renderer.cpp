#include "vulkan_renderer.hpp"
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
	createRenderPass();
	createFramebuffers();
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
	destroyFramebuffers();
	destroyRenderPass();
	destroyImages();
	destroySwapchain();
}

// VulkanRenderer_default::createSwapchain
void VulkanRenderer_default::createSwapchain() {
	// create swapchain
	vulkanSwapchainCreate(context.device, surface, &swapchain);
	// get frames count
	framesCount = (uint32_t)swapchain.images.size();
}

// VulkanRenderer_default::createImages
void VulkanRenderer_default::createImages() {
	// create color attachment image views
	colorAttachmentImageViews.resize(framesCount);
	for (uint32_t i = 0; i < framesCount; i++) {
		// VkImageViewCreateInfo
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = VK_NULL_HANDLE;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = swapchain.images[i];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = swapchain.surfaceFormat.format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		VKT_CHECK(vkCreateImageView(context.device.device, &imageViewCreateInfo, VK_NULL_HANDLE, &colorAttachmentImageViews[i]));
		assert(colorAttachmentImageViews[i]);
	}
	// create depth-stencil attachment images
	depthStencilAttachmentImages.resize(framesCount);
	depthStencilAttachmentAllocations.resize(framesCount);
	for (uint32_t i = 0; i < framesCount; i++) {
		// VkImageCreateInfo - depth and stencil
		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = VK_NULL_HANDLE;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT; // must be the same as imageViewCreateInfo.format (see below)
		imageCreateInfo.extent.width = swapchain.surfaceCapabilities.currentExtent.width;
		imageCreateInfo.extent.height = swapchain.surfaceCapabilities.currentExtent.height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
		imageCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		// VmaAllocationCreateInfo
		VmaAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocCreateInfo.flags = 0;

		// vmaCreateImage
		VKT_CHECK(vmaCreateImage(context.device.allocator, &imageCreateInfo, &allocCreateInfo, &depthStencilAttachmentImages[i], &depthStencilAttachmentAllocations[i], VK_NULL_HANDLE));
		assert(depthStencilAttachmentImages[i]);
		assert(depthStencilAttachmentAllocations[i]);
	}
	// create depth-stencil attachment image views
	depthStencilAttachmentImageViews.resize(framesCount);
	for (uint32_t i = 0; i < framesCount; i++) {
		// VkImageViewCreateInfo
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = VK_NULL_HANDLE;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = depthStencilAttachmentImages[i];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT; // must be the same as imageCreateInfo.format (see upper)
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		VKT_CHECK(vkCreateImageView(context.device.device, &imageViewCreateInfo, VK_NULL_HANDLE, &depthStencilAttachmentImageViews[i]));
		assert(depthStencilAttachmentImageViews[i]);
	}
}

// VulkanRenderer_default::createRenderPass
void VulkanRenderer_default::createRenderPass() {
	// VkAttachmentDescription - color
	std::array<VkAttachmentDescription, 2> attachmentDescriptions;
	// color attachment
	attachmentDescriptions[0].flags = 0;
	attachmentDescriptions[0].format = swapchain.surfaceFormat.format;
	attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// depth-stencil attachment
	attachmentDescriptions[1].flags = 0;
	attachmentDescriptions[1].format = VK_FORMAT_D24_UNORM_S8_UINT;
	attachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// VkAttachmentReference - color
	std::array<VkAttachmentReference, 1> colorAttachmentReferences;
	colorAttachmentReferences[0].attachment = 0;
	colorAttachmentReferences[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// VkAttachmentReference - depth-stencil
	VkAttachmentReference depthStencilAttachmentReference{};
	depthStencilAttachmentReference.attachment = 1;
	depthStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// VkSubpassDescription - subpassDescriptions
	std::array<VkSubpassDescription, 1> subpassDescriptions;
	subpassDescriptions[0].flags = 0;
	subpassDescriptions[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescriptions[0].inputAttachmentCount = 0;
	subpassDescriptions[0].pInputAttachments = VK_NULL_HANDLE;
	subpassDescriptions[0].colorAttachmentCount = (uint32_t)colorAttachmentReferences.size();
	subpassDescriptions[0].pColorAttachments = colorAttachmentReferences.data();
	subpassDescriptions[0].pResolveAttachments = VK_NULL_HANDLE;
	subpassDescriptions[0].pDepthStencilAttachment = &depthStencilAttachmentReference;
	subpassDescriptions[0].preserveAttachmentCount = 0;
	subpassDescriptions[0].pPreserveAttachments = VK_NULL_HANDLE;

	// VkRenderPassCreateInfo
	VkRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = (uint32_t)attachmentDescriptions.size();
	renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
	renderPassCreateInfo.subpassCount = (uint32_t)subpassDescriptions.size();
	renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
	VKT_CHECK(vkCreateRenderPass(context.device.device, &renderPassCreateInfo, VK_NULL_HANDLE, &renderPass));
	assert(renderPass);
}

// VulkanRenderer_default::createFramebuffers
void VulkanRenderer_default::createFramebuffers() {
	// create image views
	framebuffers.resize(framesCount);
	for (uint32_t i = 0; i < framesCount; i++) {
		// image views
		VkImageView imageViews[] = { colorAttachmentImageViews[i], depthStencilAttachmentImageViews[i] };
	
		// VkFramebufferCreateInfo
		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = VK_NULL_HANDLE;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = VKT_ARRAY_ELEMENTS_COUNT(imageViews);
		framebufferCreateInfo.pAttachments = imageViews;
		framebufferCreateInfo.width = swapchain.surfaceCapabilities.currentExtent.width;
		framebufferCreateInfo.height = swapchain.surfaceCapabilities.currentExtent.height;
		framebufferCreateInfo.layers = 1;
		VKT_CHECK(vkCreateFramebuffer(context.device.device, &framebufferCreateInfo, VK_NULL_HANDLE, &framebuffers[i]));
		assert(framebuffers[i]);
	}
}

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
	// create all shaders
	for (uint32_t materialUsage = VULKAN_MATERIAL_USAGE_BEGIN_RANGE; materialUsage <= VULKAN_MATERIAL_USAGE_END_RANGE; materialUsage++) {
		// create mesh object shaders
		vulkanShaderCreate(context.device,
			shaders_mesh_obj_files_vert[materialUsage],
			shaders_mesh_obj_files_frag[materialUsage],
			&shader_mesh_obj[materialUsage]);
		// create mesh object skin shaders
		vulkanShaderCreate(context.device,
			shaders_mesh_obj_skin_files_vert[materialUsage],
			shaders_mesh_obj_skin_files_frag[materialUsage],
			&shader_mesh_obj_skin[materialUsage]);
	}
}

// VulkanRenderer_default::createPipelines
void VulkanRenderer_default::createPipelines() {
	// create all pipelines
	for (uint32_t materialUsage = VULKAN_MATERIAL_USAGE_BEGIN_RANGE; materialUsage <= VULKAN_MATERIAL_USAGE_END_RANGE; materialUsage++) {
		for (uint32_t topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; topology <= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY; topology++) {
			// create pipeline mesh object
			vulkanPipelineCreate(context.device, shader_mesh_obj[materialUsage], context.pipelineLayout, renderPass, 0,
				(VkPrimitiveTopology)topology, VK_POLYGON_MODE_FILL,
				VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P4_T2_N3), vertexBindingDescriptions_P4_T2_N3,
				VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P4_T2_N3), vertexInputAttributeDescriptions_P4_T2_N3,
				VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
				&pipeline_mesh_obj[materialUsage][topology]);
			// create pipeline mesh object (wireframe)
			vulkanPipelineCreate(context.device, shader_mesh_obj[materialUsage], context.pipelineLayout, renderPass, 0,
				(VkPrimitiveTopology)topology, VK_POLYGON_MODE_LINE,
				VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P4_T2_N3), vertexBindingDescriptions_P4_T2_N3,
				VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P4_T2_N3), vertexInputAttributeDescriptions_P4_T2_N3,
				VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
				&pipeline_mesh_obj_wf[materialUsage][topology]);
			// create pipeline mesh object skin
			vulkanPipelineCreate(context.device, shader_mesh_obj_skin[materialUsage], context.pipelineLayout, renderPass, 0,
				(VkPrimitiveTopology)topology, VK_POLYGON_MODE_FILL,
				VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P4_T2_N3), vertexBindingDescriptions_P4_T2_N3,
				VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P4_T2_N3), vertexInputAttributeDescriptions_P4_T2_N3,
				VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
				&pipeline_mesh_obj_skin[materialUsage][topology]);
			// create pipeline mesh object skin (wireframe)
			vulkanPipelineCreate(context.device, shader_mesh_obj_skin[materialUsage], context.pipelineLayout, renderPass, 0,
				(VkPrimitiveTopology)topology, VK_POLYGON_MODE_LINE,
				VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions_P4_T2_N3), vertexBindingDescriptions_P4_T2_N3,
				VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions_P4_T2_N3), vertexInputAttributeDescriptions_P4_T2_N3,
				VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates_default), pipelineColorBlendAttachmentStates_default,
				&pipeline_mesh_obj_skin_wf[materialUsage][topology]);
		}
	}
}

// VulkanRenderer_default::destroySwapchain
void VulkanRenderer_default::destroySwapchain() {
	// destroy swapchain
	vulkanSwapchainDestroy(context.device, swapchain);
	// clear frames count
	frameIndex = 0;
	framesCount = 0;
}

// VulkanRenderer_default::destroyImages
void VulkanRenderer_default::destroyImages() {
	// destroy images
	for (uint32_t i = 0; i < colorAttachmentImageViews.size(); i++) {
		// destroy color attachment image views
		vkDestroyImageView(context.device.device, colorAttachmentImageViews[i], VK_NULL_HANDLE);
		colorAttachmentImageViews[i] = VK_NULL_HANDLE;
		// destroy depth-stencil attachment image views
		vkDestroyImageView(context.device.device, depthStencilAttachmentImageViews[i], VK_NULL_HANDLE);
		depthStencilAttachmentImageViews[i] = VK_NULL_HANDLE;
		// destroy depth-stencil attachment images
		vmaDestroyImage(context.device.allocator, depthStencilAttachmentImages[i], depthStencilAttachmentAllocations[i]);
		depthStencilAttachmentImages[i] = VK_NULL_HANDLE;
		depthStencilAttachmentAllocations[i] = {};
	}
}

// VulkanRenderer_default::destroyRenderPass
void VulkanRenderer_default::destroyRenderPass() {
	// destroy render pass
	vkDestroyRenderPass(context.device.device, renderPass, VK_NULL_HANDLE);
	renderPass = VK_NULL_HANDLE;
}

// VulkanRenderer_default::destroyFramebuffers
void VulkanRenderer_default::destroyFramebuffers() {
	// destroy framebuffers
	for (auto& framebuffer : framebuffers) {
		vkDestroyFramebuffer(context.device.device, framebuffer, VK_NULL_HANDLE);
		framebuffer = VK_NULL_HANDLE;
	}
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
	for (uint32_t materialUsage = VULKAN_MATERIAL_USAGE_BEGIN_RANGE; materialUsage <= VULKAN_MATERIAL_USAGE_END_RANGE; materialUsage++) {
		vulkanShaderDestroy(context.device, shader_mesh_obj_skin[materialUsage]);
		vulkanShaderDestroy(context.device, shader_mesh_obj[materialUsage]);
	}
}

// VulkanRenderer_default::destroyPipelines
void VulkanRenderer_default::destroyPipelines() {
	// destroy all pipelines
	for (uint32_t materialUsage = VULKAN_MATERIAL_USAGE_BEGIN_RANGE; materialUsage <= VULKAN_MATERIAL_USAGE_END_RANGE; materialUsage++) {
		for (uint32_t topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; topology <= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY; topology++) {
			vulkanPipelineDestroy(context.device, pipeline_mesh_obj_skin_wf[materialUsage][topology]);
			vulkanPipelineDestroy(context.device, pipeline_mesh_obj_skin[materialUsage][topology]);
			vulkanPipelineDestroy(context.device, pipeline_mesh_obj_wf[materialUsage][topology]);
			vulkanPipelineDestroy(context.device, pipeline_mesh_obj[materialUsage][topology]);
		}
	}
}

// VulkanRenderer_default::reinitialize
void VulkanRenderer_default::reinitialize() {
	// wait device
	VKT_CHECK(vkQueueWaitIdle(context.device.queueGraphics));
	// destroy all related handles
	destroyPipelines();
	destroyFramebuffers();
	destroyRenderPass();
	destroyImages();
	destroySwapchain();
	// create all related handles
	createSwapchain();
	createImages();
	createRenderPass();
	createFramebuffers();
	createPipelines();
}

// VulkanRenderer_default::getViewSize
uint32_t VulkanRenderer_default::getViewHeight(){
	return swapchain.surfaceCapabilities.currentExtent.height;
}

// VulkanRenderer_default::getViewWidth
uint32_t VulkanRenderer_default::getViewWidth() {
	return swapchain.surfaceCapabilities.currentExtent.width;
}

// VulkanRenderer_default::getViewAspect
float VulkanRenderer_default::getViewAspect() {
	return float(swapchain.surfaceCapabilities.currentExtent.width) / 
		float(swapchain.surfaceCapabilities.currentExtent.height);
}

// VulkanRenderer_default::drawScene
void VulkanRenderer_default::drawScene(VulkanScene* scene) 
{
	// acquire next frame index
	uint32_t imageIndex{};
	VKT_CHECK(vkAcquireNextImageKHR(context.device.device, swapchain.swapchain, UINT64_MAX, presentSemaphores[frameIndex].semaphore, VK_NULL_HANDLE, &imageIndex));

	// VkCommandBufferBeginInfo
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = VK_NULL_HANDLE;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional
	VKT_CHECK(vkBeginCommandBuffer(commandBuffers[frameIndex].commandBuffer, &commandBufferBeginInfo));

	// scene before render
	beforeRenderPass(commandBuffers[frameIndex], scene);

	// VkClearValue
	VkClearValue clearColors[2];
	clearColors[0].color = { 0.0f, 0.125f, 0.3f, 1.0f };
	clearColors[1].depthStencil.depth = 1.0f;
	clearColors[1].depthStencil.stencil = 0;

	// VkRenderPassBeginInfo
	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = VK_NULL_HANDLE;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = framebuffers[frameIndex];
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = swapchain.surfaceCapabilities.currentExtent;
	renderPassBeginInfo.clearValueCount = VKT_ARRAY_ELEMENTS_COUNT(clearColors);
	renderPassBeginInfo.pClearValues = clearColors;
	vkCmdBeginRenderPass(commandBuffers[frameIndex].commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	// VkViewport
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = (float)swapchain.surfaceCapabilities.currentExtent.height;
	viewport.width = (float)swapchain.surfaceCapabilities.currentExtent.width;
	viewport.height = -(float)swapchain.surfaceCapabilities.currentExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffers[frameIndex].commandBuffer, 0, 1, &viewport);

	// VkRect2D (scissor)
	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapchain.surfaceCapabilities.currentExtent;
	vkCmdSetScissor(commandBuffers[frameIndex].commandBuffer, 0, 1, &scissor);
	
	// set line width
	vkCmdSetLineWidth(commandBuffers[frameIndex].commandBuffer, 1.0f);

	// present render pass
	presentSubPass(commandBuffers[frameIndex], scene);

	// end render pass
	vkCmdEndRenderPass(commandBuffers[frameIndex].commandBuffer);

	// after render pass
	afterRenderPass(commandBuffers[frameIndex], scene);

	// end command buffer
	VKT_CHECK(vkEndCommandBuffer(commandBuffers[frameIndex].commandBuffer));

	// VkSubmitInfo
	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[frameIndex].commandBuffer;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &presentSemaphores[frameIndex].semaphore;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderSemaphores[frameIndex].semaphore;
	VKT_CHECK(vkQueueSubmit(context.device.queueGraphics, 1, &submitInfo, VK_NULL_HANDLE));

	// VkPresentInfoKHR
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = VK_NULL_HANDLE;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderSemaphores[frameIndex].semaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain.swapchain;
	presentInfo.pImageIndices = &imageIndex;
	VKT_CHECK(vkQueuePresentKHR(context.device.queueGraphics, &presentInfo));
	VKT_CHECK(vkQueueWaitIdle(context.device.queueGraphics));

	// update frame index
	frameIndex = (frameIndex + 1) % framesCount;
}

// VulkanRenderer_default::beforeRenderPass
void VulkanRenderer_default::beforeRenderPass(VulkanCommandBuffer& commandBuffer, VulkanScene* scene) 
{
	// scene before render pass
	scene->update(commandBuffer);
}

// VulkanRenderer_default::insideRenderPass
void VulkanRenderer_default::presentSubPass(VulkanCommandBuffer& commandBuffer, VulkanScene* scene)
{
	// bind scene data to shader
	scene->bind(commandBuffers[frameIndex]);
	// draw models
	for (auto& model : scene->models) {
		// bind model data to shader
		model->bind(commandBuffers[frameIndex]);
		// draw meshes
		if (model->visible) {
			for (auto& mesh : model->meshes) {
				// bind pipeline
				assert(mesh->primitiveTopology != VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
				assert(mesh->primitiveTopology != VK_PRIMITIVE_TOPOLOGY_PATCH_LIST);
// 				vkCmdBindPipeline(commandBuffers[frameIndex].commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
// 					pipeline_obj[mesh->primitiveTopology].pipeline);
// 				// draw mesh
// 				mesh->draw(commandBuffers[frameIndex]);
			}
		}
		// draw debug meshes
		if (model->visibleDebug) {
			for (auto& mesh : model->meshes_debug) {
				// bind pipeline
				assert(mesh->primitiveTopology != VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
				assert(mesh->primitiveTopology != VK_PRIMITIVE_TOPOLOGY_PATCH_LIST);
// 				vkCmdBindPipeline(commandBuffers[frameIndex].commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
// 					pipeline_debug[mesh->primitiveTopology].pipeline);
// 				// draw mesh
// 				mesh->draw(commandBuffers[frameIndex]);
			}
		}
	}
}

// VulkanRenderer_default::afterRenderPass
void VulkanRenderer_default::afterRenderPass(VulkanCommandBuffer& commandBuffer, VulkanScene* scene)
{
}
