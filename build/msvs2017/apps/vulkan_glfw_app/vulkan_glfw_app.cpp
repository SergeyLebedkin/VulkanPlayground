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
	VulkanRender* renderer = new VulkanRender(window,
		enabledInstanceLayerNames, enabledInstanceExtensionNames,
		enabledDeviceExtensionNames, physicalDeviceFeatures);

	std::vector<VulkanMesh*> meshes;
	std::vector<VulkanMesh*> meshesDebug;
	std::vector<VulkanImage*> images;
	//loadMesh_obj(vulkanRender->device, shader_obj, shader_line, sampler, "models/rock/rock.obj", "models/rock", &meshes, &meshesLines, &images);
	//loadMesh_obj(*renderer, "models/rock/rock.obj", "models/rock", &meshes, &meshesDebug, &images);
	loadMesh_obj(*renderer, "models/tea/tea.obj", "models/tea", &meshes, &meshesDebug, &images);
	//loadMesh_obj(device, shader_obj, shader_line, sampler, "models/train/train.obj", "models", &meshes, &meshesLines, &images);

	// create GUI mesh
	VulkanMesh* meshGUI = new VulkanMesh_gui(renderer->device, renderer->descriptorSetLayout_default, vertices);
	meshGUI->setImage(*images[0], renderer->samplerDefault, 0);

	// matrices
	float viewWidth = (float)renderer->swapchain.surfaceCapabilities.currentExtent.width;
	float viewHeight = (float)renderer->swapchain.surfaceCapabilities.currentExtent.height;
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
		vulkanSwapchainBeginFrame(renderer->device, renderer->swapchain, renderer->presentSemaphore, &frameIndex);
		vulkanCommandBufferBegin(renderer->device, renderer->commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		
		// VkClearValue
		VkClearValue clearColors[2];
		clearColors[0].color = { 0.0f, 0.125f, 0.3f, 1.0f };
		clearColors[1].depthStencil.depth = 1.0f;
		clearColors[1].depthStencil.stencil = 0;

		// VkRenderPassBeginInfo
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = VK_NULL_HANDLE;
		renderPassBeginInfo.renderPass = renderer->swapchain.renderPass;
		renderPassBeginInfo.framebuffer = renderer->swapchain.framebuffers[frameIndex];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = { (uint32_t)viewWidth, (uint32_t)viewHeight };
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearColors;

		// VkViewport - viewport
		VkViewport viewport{};
		viewport.x = 0.0f; 
		viewport.y = viewHeight;
		viewport.width = viewWidth;
		viewport.height = -viewHeight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// VkRect2D - scissor
		VkRect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = (uint32_t)viewWidth;
		scissor.extent.height = (uint32_t)viewHeight;

		// GO RENDER
		vkCmdBeginRenderPass(renderer->commandBuffer.commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdSetViewport(renderer->commandBuffer.commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(renderer->commandBuffer.commandBuffer, 0, 1, &scissor);
		vkCmdSetLineWidth(renderer->commandBuffer.commandBuffer, 1.0f);
		
		// draw obj
		for (auto mesh : meshes) {
			//mesh->draw(renderer->pipeline_obj, renderer->commandBuffer, matProj, matView, matModl);
			mesh->draw(renderer->pipeline_obj_wf, renderer->commandBuffer, matProj, matView, matModl);
		}

		// draw lines
		//for (auto mesh : meshesDebug)
		//	mesh->draw(renderer->pipeline_line, renderer->commandBuffer, matProj, matView, matModl);

		//meshGUI->draw(pipeline_default, commandBuffer, matProjGUI, matViewGUI, matModlGUI);
		//meshGUI->draw(pipeline_obj_wf, commandBuffer, matProjGUI, matViewGUI, matModlGUI);

		// END RENDER
		vkCmdEndRenderPass(renderer->commandBuffer.commandBuffer);

		// vkEndCommandBuffer
		VKT_CHECK(vkEndCommandBuffer(renderer->commandBuffer.commandBuffer));
		
		vulkanQueueSubmit(renderer->device, renderer->commandBuffer, &renderer->presentSemaphore, &renderer->renderSemaphore);
		vulkanSwapchainEndFrame(renderer->device, renderer->swapchain, renderer->renderSemaphore, frameIndex);
		glfwPollEvents();
	}

	// delete images
	for (auto image : images) {
		vulkanImageDestroy(renderer->device, *image);
		delete image;
	}
	images.clear();

	// delete meshes
	for (auto mesh : meshesDebug) delete mesh;
	meshesDebug.clear();
	for (auto mesh : meshes) delete mesh;
	meshes.clear();
	delete meshGUI;

	delete renderer;

	// destroy GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
