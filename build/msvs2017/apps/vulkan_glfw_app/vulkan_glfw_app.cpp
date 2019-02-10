#include <vktoolkit.hpp>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <iostream>
#include <chrono>
#include <string>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable: 4996)
#include <stb_image.h>
#include <stb_image_write.h>
#pragma warning(pop)

int main(void)
{
	// init GLFW
	glfwInit();
	if (!glfwVulkanSupported()) assert(0 && "Vulkan not supported");
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);

	// vulkan extensions
	std::vector<const char *> enabledInstanceLayerNames{ "VK_LAYER_LUNARG_standard_validation" };
	std::vector<const char *> enabledInstanceExtensionNames{ VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME };
	std::vector<const char *> enabledDeviceExtensionNames{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// VkPhysicalDeviceFeatures
	VkPhysicalDeviceFeatures physicalDeviceFeatures{};
	physicalDeviceFeatures.depthBounds = VK_TRUE;
	physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;

	// init vulkan
	VulkanInstance      instance{};
	VulkanSurface       surface{};
	VulkanDevice        device{};
	VulkanSwapchain     swapchain{};
	VulkanSemaphore     renderSemaphore{};
	VulkanSemaphore     presentSemaphore{};
	VulkanCommandBuffer commandBuffer{};
	VulkanPipeline      pipeline{};
	vulkanInstanceCreate(enabledInstanceLayerNames, enabledInstanceExtensionNames, &instance);
	glfwCreateWindowSurface(instance.instance, window, NULL, &surface.surface);
	vulkanDeviceCreate(instance, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, physicalDeviceFeatures, enabledDeviceExtensionNames, &device);
	vulkanSwapchainCreate(device, surface, &swapchain);
	vulkanSemaphoreCreate(device, &renderSemaphore);
	vulkanSemaphoreCreate(device, &presentSemaphore);
	vulkanCommandBufferAllocate(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &commandBuffer);

	// VkVertexInputBindingDescription
	VkVertexInputBindingDescription vertexBindingDescriptions[] {
		{ 0, 10 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX },
	};

	// VkVertexInputAttributeDescription
	VkVertexInputAttributeDescription vertexInputAttributeDescriptions[] {
		{ 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT,  0 }, // position
		{ 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 16 }, // color
		{ 2, 0, VK_FORMAT_R32G32_SFLOAT      , 32 }, // texCoord
	};

	// VkDescriptorSetLayoutBinding
	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[] {
		{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, VK_NULL_HANDLE }, // texture
		{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1, VK_SHADER_STAGE_VERTEX_BIT  , VK_NULL_HANDLE }, // buffer
	};

	// VkPipelineColorBlendAttachmentState
	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentStates[] {
		{ // first attachement
			VK_FALSE,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		}
	};

	// create pipeline
	vulkanPipelineCreate(device, swapchain.renderPass, 0,
		"shaders/base.vert.spv", "shaders/base.frag.spv",
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions), vertexBindingDescriptions,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions), vertexInputAttributeDescriptions,
		VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings), descriptorSetLayoutBindings,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates), pipelineColorBlendAttachmentStates,
		&pipeline
	);

	// load image from file
	int width = 0, height = 0, channels = 0;
	stbi_uc* texData = stbi_load("textures/texture.png", &width, &height, &channels, 4);

	VulkanImage image1{};
	VulkanImage image2{};
	VulkanSampler sampler{};
	vulkanImageCreate(device, VK_FORMAT_R8G8B8A8_UINT, width, height, 1, &image1);
	vulkanImageCreate(device, VK_FORMAT_R8G8B8A8_UINT, width, height, 1, &image2);
	vulkanSamplerCreate(device, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_TRUE, &sampler);

	vulkanImageWrite(device, image1, 0, texData);
	vulkanImageBuildMipmaps(device, image1);

	for (uint32_t i = 0; i < image1.mipLevels; i++)
		vulkanImageCopy(device, image1, i, image2, i);

// 	for (uint32_t i = 0; i < image1.mipLevels; i++) {
// 		const std::string fileName = "textures/texture_" + std::to_string(i) + ".png";
// 		vulkanImageRead(device, image2, i, texData);
// 		stbi_write_png(fileName.data(), std::max(1, width >> i), std::max(1, height >> i), 4, texData, 0);
// 	}

	vulkanSamplerDestroy(device, sampler);
	vulkanImageDestroy(device, image2);
	vulkanImageDestroy(device, image1);

	VulkanBuffer        bufferIndex{};
	VulkanBuffer        bufferVertex{};
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 413, &bufferVertex);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 619, &bufferIndex);

	vulkanBufferWrite(device, bufferVertex, 0, 100, texData);
	memset(texData, 100, width * height * 4);
	vulkanBufferRead(device, bufferVertex, 0, 100, texData);

	vulkanBufferDestroy(device, bufferIndex);
	vulkanBufferDestroy(device, bufferVertex);

	stbi_image_free(texData);

	// main loop
	while (!glfwWindowShouldClose(window))
	{
		uint32_t frameIndex = 0;
		vulkanSwapchainBeginFrame(device, swapchain, presentSemaphore, &frameIndex);
		vulkanCommandBufferBegin(device, commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		
		// VkClearValue
		VkClearValue clearColors[2];
		clearColors[0].color = { 0.0f, 0.125f, 0.3f, 1.0f };
		clearColors[1].depthStencil.depth = 1.0f;
		clearColors[1].depthStencil.stencil = 0;

		// VkRenderPassBeginInfo
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = VK_NULL_HANDLE;
		renderPassBeginInfo.renderPass = swapchain.renderPass;
		renderPassBeginInfo.framebuffer = swapchain.framebuffers[frameIndex];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = swapchain.surfaceCapabilities.currentExtent;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearColors;

		// VkViewport - viewport
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = (float)swapchain.surfaceCapabilities.currentExtent.height;
		viewport.width = (float)swapchain.surfaceCapabilities.currentExtent.width;
		viewport.height = -(float)swapchain.surfaceCapabilities.currentExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// VkRect2D - scissor
		VkRect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = swapchain.surfaceCapabilities.currentExtent.width;
		scissor.extent.height = swapchain.surfaceCapabilities.currentExtent.height;

		// GO RENDER
		vkCmdBeginRenderPass(commandBuffer.commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdSetViewport(commandBuffer.commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer.commandBuffer, 0, 1, &scissor);
		vkCmdBindPipeline(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
		vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &pipeline.descriptorSet, 0, VK_NULL_HANDLE);

		// END RENDER
		vkCmdEndRenderPass(commandBuffer.commandBuffer);

		// vkEndCommandBuffer
		VKT_CHECK(vkEndCommandBuffer(commandBuffer.commandBuffer));
		
		vulkanQueueSubmit(device, commandBuffer, &presentSemaphore, &renderSemaphore);
		vulkanSwapchainEndFrame(device, swapchain, renderSemaphore, frameIndex);
		glfwPollEvents();
	}

	// destroy vulkan
	vulkanPipelineDestroy(device, pipeline);
	vulkanCommandBufferFree(device, commandBuffer);
	vulkanSemaphoreDestroy(device, presentSemaphore);
	vulkanSemaphoreDestroy(device, renderSemaphore);
	vulkanSwapchainDestroy(device, swapchain);
	vkDestroySurfaceKHR(instance.instance, surface.surface, NULL);
	vulkanDeviceDestroy(device);
	vulkanInstanceDestroy(instance);

	// destroy GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
