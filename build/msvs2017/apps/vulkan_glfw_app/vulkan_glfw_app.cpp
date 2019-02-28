#include "vulkan_renderer.hpp"
#include "vulkan_assets.hpp"
#include "vulkan_scene.hpp"
#include "time_measure.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

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

	// create assets manages
	VulkanAssetManager* assetsManager = new VulkanAssetManager(renderer);
	assetsManager->loadFileObj("models/train/train.obj", "models/train");
	//assetsManager->loadFileObj("models/rock/rock.obj", "models/rock");
	//assetsManager->loadFileObj("models/tea/tea.obj", "models/tea");

	// get loaded models
	VulkanModel* modelTrain = assetsManager->createOrDefaut_Model("models/train/train.obj");
	//VulkanModel* modelRock = assetsManager->createOrDefaut_Model("models/rock/rock.obj");
	//VulkanModel* modelTea = assetsManager->createOrDefaut_Model("models/tea/tea.obj");
	
	// create scene
	float viewWidth = (float)renderer->swapchain.surfaceCapabilities.currentExtent.width;
	float viewHeight = (float)renderer->swapchain.surfaceCapabilities.currentExtent.height;

	// create scene
	VulkanScene* scene = new VulkanScene(renderer->device, renderer->pipelineLayout, renderer->descriptorSetLayout_scene);
	scene->matView = glm::lookAt(glm::vec3(0.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scene->matProj = glm::perspective(glm::radians(45.0f), viewWidth / viewHeight, 0.1f, 10.f);
	//scene->models.push_back(modelRock);
	//scene->models.push_back(modelTea);
	scene->models.push_back(modelTrain);

	// create time stamp
	TimeStamp timeStamp;
	timeStampReset(timeStamp);

	// main loop
	while (!glfwWindowShouldClose(window))
	{
		timeStampTick(timeStamp);
		timeStampPrint(std::cout, timeStamp, 1.0f);

		// rotate model
		modelRock->matModel = glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / 1.0f)), timeStamp.accumTime, glm::vec3(0.0f, 1.0f, 0.0f));

		// begin frame
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

		scene->beforeRender(renderer->commandBuffer);

		// GO RENDER
		vkCmdBeginRenderPass(renderer->commandBuffer.commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// set parameters
		vkCmdSetViewport(renderer->commandBuffer.commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(renderer->commandBuffer.commandBuffer, 0, 1, &scissor);
		vkCmdSetLineWidth(renderer->commandBuffer.commandBuffer, 1.0f);

		// DRAW
		scene->render(renderer->commandBuffer);

		// END RENDER
		vkCmdEndRenderPass(renderer->commandBuffer.commandBuffer);

		scene->afterRender(renderer->commandBuffer);

		// vkEndCommandBuffer
		VKT_CHECK(vkEndCommandBuffer(renderer->commandBuffer.commandBuffer));
		
		// and frame
		vulkanQueueSubmit(renderer->device, renderer->commandBuffer, &renderer->presentSemaphore, &renderer->renderSemaphore);
		vulkanSwapchainEndFrame(renderer->device, renderer->swapchain, renderer->renderSemaphore, frameIndex);
		glfwPollEvents();
	}

	delete scene;
	delete modelTea;
	delete modelRock;

	delete assetsManager;
	delete renderer;

	// destroy GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
