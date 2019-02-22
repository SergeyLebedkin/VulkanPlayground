#include "vulkan_renderer.hpp"
#include "vulkan_loaders.hpp"
#include "time_measure.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

// vertex array
std::vector<VertexStruct_P4_C4_T2> vertices = {
	{ +1.0f, -1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +1.0f, +1.0, /**/+1, +0 },
	{ +1.0f, +1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +1.0f, +1.0, /**/+1, +1 },
	{ -1.0f, -1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +1.0f, +1.0, /**/+0, +0 },
	{ -1.0f, +1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +1.0f, +1.0, /**/+0, +1 },
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

	// create vulkan renderer
	VulkanRender* vulkanRender = new VulkanRender(window,
		enabledInstanceLayerNames, enabledInstanceExtensionNames,
		enabledDeviceExtensionNames, physicalDeviceFeatures);

	std::vector<VulkanMesh*> meshes;
	std::vector<VulkanMesh*> meshesLines;
	std::vector<VulkanImage*> images;
	//loadMesh_obj(vulkanRender->device, shader_obj, shader_line, sampler, "models/rock/rock.obj", "models/rock", &meshes, &meshesLines, &images);
	loadMesh_obj(vulkanRender->device, vulkanRender->shader_obj, vulkanRender->shader_line, vulkanRender->samplerDefault, vulkanRender->imageDefault, "models/tea/tea.obj", "models/tea", &meshes, &meshesLines, &images);
	//loadMesh_obj(device, shader_obj, shader_line, sampler, "models/train/train.obj", "models", &meshes, &meshesLines, &images);

	// create GUI mesh
	VulkanMesh* meshGUI = new VulkanMesh_gui(vulkanRender->device, vulkanRender->shader_default, vertices);
	meshGUI->setImage(*images[0], vulkanRender->samplerDefault, 0);

	// matrices
	float viewWidth = (float)vulkanRender->swapchain.surfaceCapabilities.currentExtent.width;
	float viewHeight = (float)vulkanRender->swapchain.surfaceCapabilities.currentExtent.height;
	glm::mat4 matModl = glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/1.0f)), 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 matView = glm::lookAt(glm::vec3(0.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 matProj = glm::perspective(glm::radians(45.0f), viewWidth / viewHeight, 0.1f, 10.f);

	glm::mat4 matModlGUI = glm::mat4(1.0f);
	glm::mat4 matViewGUI = glm::mat4(1.0f);
	glm::mat4 matProjGUI = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 1.0f);

	TimeStamp timeStamp;
	timeStampReset(timeStamp);

	// main loop
	while (!glfwWindowShouldClose(window))
	{
		timeStampTick(timeStamp);
		timeStampPrint(std::cout, timeStamp, 1.0f);

		matModl = glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / 1.0f)), timeStamp.accumTime, glm::vec3(0.0f, 1.0f, 0.0f));

		uint32_t frameIndex = 0;
		vulkanSwapchainBeginFrame(vulkanRender->device, vulkanRender->swapchain, vulkanRender->presentSemaphore, &frameIndex);
		vulkanCommandBufferBegin(vulkanRender->device, vulkanRender->commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		
		// VkClearValue
		VkClearValue clearColors[2];
		clearColors[0].color = { 0.0f, 0.125f, 0.3f, 1.0f };
		clearColors[1].depthStencil.depth = 1.0f;
		clearColors[1].depthStencil.stencil = 0;

		// VkRenderPassBeginInfo
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = VK_NULL_HANDLE;
		renderPassBeginInfo.renderPass = vulkanRender->swapchain.renderPass;
		renderPassBeginInfo.framebuffer = vulkanRender->swapchain.framebuffers[frameIndex];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = vulkanRender->swapchain.surfaceCapabilities.currentExtent;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearColors;

		// VkViewport - viewport
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = (float)vulkanRender->swapchain.surfaceCapabilities.currentExtent.height;
		viewport.width = (float)vulkanRender->swapchain.surfaceCapabilities.currentExtent.width;
		viewport.height = -(float)vulkanRender->swapchain.surfaceCapabilities.currentExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// VkRect2D - scissor
		VkRect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = vulkanRender->swapchain.surfaceCapabilities.currentExtent.width;
		scissor.extent.height = vulkanRender->swapchain.surfaceCapabilities.currentExtent.height;

		// GO RENDER
		vkCmdBeginRenderPass(vulkanRender->commandBuffer.commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdSetViewport(vulkanRender->commandBuffer.commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(vulkanRender->commandBuffer.commandBuffer, 0, 1, &scissor);
		vkCmdSetLineWidth(vulkanRender->commandBuffer.commandBuffer, 1.0f);
		
		// draw obj
		for (auto mesh : meshes) {
			mesh->draw(vulkanRender->pipeline_obj, vulkanRender->commandBuffer, matProj, matView, matModl);
			//mesh->draw(pipeline_obj_wf, commandBuffer, matProj, matView, matModl);
		}

		// draw lines
		//for (auto mesh : meshesLines)
		//	mesh->draw(pipeline_line, commandBuffer, matProj, matView, matModl);

		//meshGUI->draw(pipeline_default, commandBuffer, matProjGUI, matViewGUI, matModlGUI);
		//meshGUI->draw(pipeline_obj_wf, commandBuffer, matProjGUI, matViewGUI, matModlGUI);

		// END RENDER
		vkCmdEndRenderPass(vulkanRender->commandBuffer.commandBuffer);

		// vkEndCommandBuffer
		VKT_CHECK(vkEndCommandBuffer(vulkanRender->commandBuffer.commandBuffer));
		
		vulkanQueueSubmit(vulkanRender->device, vulkanRender->commandBuffer, &vulkanRender->presentSemaphore, &vulkanRender->renderSemaphore);
		vulkanSwapchainEndFrame(vulkanRender->device, vulkanRender->swapchain, vulkanRender->renderSemaphore, frameIndex);
		glfwPollEvents();
	}

	// delete images
	for (auto image : images) {
		vulkanImageDestroy(vulkanRender->device, *image);
		delete image;
	}
	images.clear();

	// delete meshes
	for (auto mesh : meshesLines) delete mesh;
	meshesLines.clear();
	for (auto mesh : meshes) delete mesh;
	meshes.clear();
	delete meshGUI;

	delete vulkanRender;

	// destroy GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
