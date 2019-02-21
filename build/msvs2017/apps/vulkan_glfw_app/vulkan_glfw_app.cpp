#include "vulkan_loaders.hpp"
#include <iostream>
#include <chrono>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

// vertex array
VertexStruct_P4_C4_T2 vertices[] = {
	{ +1.0f, -1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +0.0f, +1.0, /**/+1, +0 },
	{ +1.0f, +1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +0.0f, +1.0, /**/+1, +1 },
	{ -1.0f, -1.0f, +0.0f, +1.0, /**/+1.0f, +0.0f, +0.0f, +1.0, /**/+0, +0 },
	{ -1.0f, +1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +0.0f, +1.0, /**/+0, +1 },
};

// index array
uint16_t indexes[] = { 0, 1, 2, 2, 1, 3 };

// main
int main(void)
{
	// init GLFW
	glfwInit();
	if (!glfwVulkanSupported()) assert(0 && "Vulkan not supported");
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Simple example", NULL, NULL);

	// vulkan extensions
	std::vector<const char *> enabledInstanceLayerNames{ "VK_LAYER_LUNARG_standard_validation" };
	std::vector<const char *> enabledInstanceExtensionNames{ VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME };
	std::vector<const char *> enabledDeviceExtensionNames{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// VkPhysicalDeviceFeatures
	VkPhysicalDeviceFeatures physicalDeviceFeatures{};
	physicalDeviceFeatures.depthBounds = VK_TRUE;
	physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	physicalDeviceFeatures.fillModeNonSolid = VK_TRUE;

	// init vulkan
	VulkanInstance      instance{};
	VulkanSurface       surface{};
	VulkanDevice        device{};
	VulkanSwapchain     swapchain{};
	VulkanSemaphore     renderSemaphore{};
	VulkanSemaphore     presentSemaphore{};
	VulkanCommandBuffer commandBuffer{};
	vulkanInstanceCreate(enabledInstanceLayerNames, enabledInstanceExtensionNames, &instance);
	glfwCreateWindowSurface(instance.instance, window, NULL, &surface.surface);
	vulkanDeviceCreate(instance, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, physicalDeviceFeatures, enabledDeviceExtensionNames, &device);
	vulkanSwapchainCreate(device, surface, &swapchain);
	vulkanSemaphoreCreate(device, &renderSemaphore);
	vulkanSemaphoreCreate(device, &presentSemaphore);
	vulkanCommandBufferAllocate(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &commandBuffer);

	// create pipelines and shaders
	VulkanPipeline pipeline_default{};
	VulkanPipeline pipeline_obj{}; 
	VulkanPipeline pipeline_obj_wf{};
	VulkanPipeline pipeline_line{};
	VulkanShader shader_default{};
	VulkanShader shader_obj{};
	VulkanShader shader_line{};
	createPipeline_default(device, swapchain.renderPass, 0, shader_default, pipeline_default);
	createPipeline_obj(device, swapchain.renderPass, 0, shader_obj, pipeline_obj, pipeline_obj_wf);
	createPipeline_line(device, swapchain.renderPass, 0, shader_line, pipeline_line);
	
	// create sampler
	VulkanSampler sampler{};
	vulkanSamplerCreate(device, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_TRUE, &sampler);

	std::vector<VulkanMesh*> meshes;
	std::vector<VulkanMesh*> meshesLines;
	std::vector<VulkanImage*> images;
	//loadMesh_obj(device, shader_obj, shader_line, sampler, "models/daisy3.obj", "models", &meshesLines, &images);
	//loadMesh_obj(device, shader_obj, shader_line, sampler, "models/rock/rock.obj", "models/rock", &meshes, &meshesLines, &images);
	loadMesh_obj(device, shader_obj, shader_line, sampler, "models/tea/tea.obj", "models/tea", &meshes, &meshesLines, &images);
	//loadMesh_obj(device, shader_obj, shader_line, sampler, "models/train/train.obj", "models", &meshes, &meshesLines, &images);

	// matrices
	float aspect = (float)swapchain.surfaceCapabilities.currentExtent.width / swapchain.surfaceCapabilities.currentExtent.height;
	glm::mat4 matModl = glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/1.0f)), 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 matView = glm::lookAt(
		glm::vec3(0.0f, 1.0f, 2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 matProj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.f);

	// main loop
	while (!glfwWindowShouldClose(window))
	{
		// get time stamps
		static uint32_t frames = 0;
		static float newTime = 0.0f;
		static float currentTime = 0.0f;
		static auto prevTimePoint = std::chrono::high_resolution_clock::now();
		static auto nextTimePoint = std::chrono::high_resolution_clock::now();
		prevTimePoint = nextTimePoint;
		nextTimePoint = std::chrono::high_resolution_clock::now();

		float delta = std::chrono::duration_cast<std::chrono::duration<float>>(nextTimePoint - prevTimePoint).count();
		newTime += delta;

		currentTime += delta;
		frames++;
		if (currentTime >= 1.0f) {
			std::cout << frames << " frames in " << currentTime << " seconds" << std::endl;
			frames = 0;
			currentTime = 0.0f;
		}

		matModl = glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / 1.0f)), newTime, glm::vec3(0.0f, 1.0f, 0.0f));

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

		// VkDeviceSize
		VkDeviceSize offsets[] = { 0, 0, 0 };

		// GO RENDER
		vkCmdBeginRenderPass(commandBuffer.commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdSetViewport(commandBuffer.commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer.commandBuffer, 0, 1, &scissor);
		vkCmdSetLineWidth(commandBuffer.commandBuffer, 1.0f);
		
		// draw obj
		for (auto mesh : meshes)
			//mesh->draw(pipeline_obj, commandBuffer, matProj, matView, matModl);
			mesh->draw(pipeline_obj_wf, commandBuffer, matProj, matView, matModl);

		// draw lines
		for (auto mesh : meshesLines)
			mesh->draw(pipeline_line, commandBuffer, matProj, matView, matModl);

		// END RENDER
		vkCmdEndRenderPass(commandBuffer.commandBuffer);

		// vkEndCommandBuffer
		VKT_CHECK(vkEndCommandBuffer(commandBuffer.commandBuffer));
		
		vulkanQueueSubmit(device, commandBuffer, &presentSemaphore, &renderSemaphore);
		vulkanSwapchainEndFrame(device, swapchain, renderSemaphore, frameIndex);
		glfwPollEvents();
	}

	// delete images
	for (auto image : images) {
		vulkanImageDestroy(device, *image);
		delete image;
	}
	images.clear();

	// delete meshes
	for (auto mesh : meshesLines) delete mesh;
	meshesLines.clear();
	for (auto mesh : meshes) delete mesh;
	meshes.clear();

	// destroy vulkan
	vulkanSamplerDestroy(device, sampler);
	vulkanPipelineDestroy(device, pipeline_line);
	vulkanPipelineDestroy(device, pipeline_obj);
	vulkanPipelineDestroy(device, pipeline_default);
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
