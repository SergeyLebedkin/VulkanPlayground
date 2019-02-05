#include <vktoolkit.hpp>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <iostream>
#include <chrono>
#include <string>

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
	physicalDeviceFeatures.wideLines = VK_TRUE;

	// init vulkan
	VulkanInstance      instance{};
	VulkanSurface       surface{};
	VulkanDevice        device{};
	VulkanSwapchain     swapchain{};
	VulkanSemaphore     renderSemaphore{};
	VulkanSemaphore     presentSemaphore{};
	VulkanCommandBuffer commandBuffer{};
	VulkanBuffer        bufferIndex{};
	VulkanBuffer        bufferVertex{};
	vulkanInstanceCreate(enabledInstanceLayerNames, enabledInstanceExtensionNames, &instance);
	glfwCreateWindowSurface(instance.instance, window, NULL, &surface.surface);
	vulkanDeviceCreate(instance, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, physicalDeviceFeatures, enabledDeviceExtensionNames, &device);
	vulkanSwapchainCreate(device, surface, &swapchain);
	vulkanSemaphoreCreate(device, &renderSemaphore);
	vulkanSemaphoreCreate(device, &presentSemaphore);
	vulkanCommandBufferAllocate(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &commandBuffer);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, 1024, &bufferVertex);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, 1024, &bufferIndex);

	uint32_t value = 12;
	VulkanBuffer buffer0{};
	VulkanBuffer buffer1{};
	VulkanBuffer buffer2{};
	VulkanBuffer buffer3{};
	vulkanBufferCreate(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY, 1024, &buffer0);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, 1024, &buffer1);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, 1024, &buffer2);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, 1024, &buffer3);

	vulkanBufferWrite(device, buffer0, 0, sizeof(value), &value);
	vulkanBufferCopy(device, buffer0, 00, buffer1, 24, sizeof(value));
	vulkanBufferCopy(device, buffer1, 24, buffer2, 32, sizeof(value));
	vulkanBufferCopy(device, buffer2, 32, buffer3, 64, sizeof(value));

	uint32_t value0 = 12;
	uint32_t value1 = 12;
	uint32_t value2 = 12;
	uint32_t value3 = 12;
	vulkanBufferRead(device, buffer0, 00, sizeof(value), &value0);
	vulkanBufferRead(device, buffer1, 24, sizeof(value), &value1);
	vulkanBufferRead(device, buffer2, 32, sizeof(value), &value2);
	vulkanBufferRead(device, buffer3, 64, sizeof(value), &value3);

	vulkanBufferDestroy(device, buffer0);
	vulkanBufferDestroy(device, buffer1);
	vulkanBufferDestroy(device, buffer2);
	vulkanBufferDestroy(device, buffer3);

	// main loop
	while (!glfwWindowShouldClose(window))
	{
		uint32_t frameIndex = 0;
		vulkanSwapchainBeginFrame(device, swapchain, presentSemaphore, &frameIndex);
		
		// VkCommandBufferBeginInfo
		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = VK_NULL_HANDLE;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional
		VKT_CHECK(vkBeginCommandBuffer(commandBuffer.ñommandBuffer, &commandBufferBeginInfo));

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

		// GO RENDER
		vkCmdBeginRenderPass(commandBuffer.ñommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// VkViewport - viewport
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = (float)swapchain.surfaceCapabilities.currentExtent.height;
		viewport.width = (float)swapchain.surfaceCapabilities.currentExtent.width;
		viewport.height = -(float)swapchain.surfaceCapabilities.currentExtent.height;
		viewport.minDepth = 0.5f;
		viewport.maxDepth = 1.0f;

		// VkRect2D - scissor
		VkRect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = swapchain.surfaceCapabilities.currentExtent.width;
		scissor.extent.height = swapchain.surfaceCapabilities.currentExtent.height;

		vkCmdEndRenderPass(commandBuffer.ñommandBuffer);

		// vkEndCommandBuffer
		VKT_CHECK(vkEndCommandBuffer(commandBuffer.ñommandBuffer));
		
		vulkanQueueSubmit(device, commandBuffer, presentSemaphore, renderSemaphore);
		vulkanSwapchainEndFrame(device, swapchain, renderSemaphore, frameIndex);
		glfwPollEvents();
	}

	// destroy vulkan
	vulkanBufferDestroy(device, bufferIndex);
	vulkanBufferDestroy(device, bufferVertex);
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
