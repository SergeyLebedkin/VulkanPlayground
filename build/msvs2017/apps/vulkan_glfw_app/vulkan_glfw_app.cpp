#include "vulkan_context.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_assets.hpp"
#include "vulkan_scene.hpp"
#include "time_measure.hpp"
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

// main
int main(int argc, char ** argv)
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

	// create vulkan context
	VulkanContext* context = new VulkanContext(
		enabledInstanceLayerNames, 
		enabledInstanceExtensionNames,
		enabledDeviceExtensionNames, 
		physicalDeviceFeatures);

	// create window surface
	VulkanSurface* surface = new VulkanSurface();
	glfwCreateWindowSurface(context->instance.instance, window, NULL, &surface->surface);

	// create vulkan renderer
	VulkanRenderer* renderer = new VulkanRenderer_default(*context, *surface);

	// create assets manages
	VulkanAssetManager* assetsManager = new VulkanAssetManager(*context);
	//assetsManager->loadFromFileObj("models/train/train.obj", "models/train");
	//assetsManager->loadFromFileObj("models/rock/rock.obj", "models/rock");
	assetsManager->loadFromFileObj("models/tea/tea.obj", "models/tea");

	// get loaded models
	//VulkanModel* model = assetsManager->createModelByMeshGroupName("models/train/train.obj");
	//VulkanModel* model = assetsManager->createModelByMeshGroupName("models/rock/rock.obj");
	VulkanModel* model = assetsManager->createModelByMeshGroupName("models/tea/tea.obj");

	// create scene
	VulkanScene* scene = new VulkanScene(*context);
	scene->matView = glm::lookAt(glm::vec3(0.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scene->matProj = glm::perspective(glm::radians(45.0f), renderer->getViewAspect(), 0.1f, 10.f);
	scene->models.push_back(model);

	// create time stamp
	TimeStamp timeStamp{};
	timeStampReset(timeStamp);

	// main loop
	while (!glfwWindowShouldClose(window))
	{
		// get time tick
		timeStampTick(timeStamp);
		timeStampPrint(std::cout, timeStamp, 1.0f);

		// rotate model
		model->matModel = glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / 1.0f)), timeStamp.accumTime, glm::vec3(0.0f, 1.0f, 0.0f));

		// draw scene
		renderer->drawScene(scene);

		glfwPollEvents();
	}

	// destroy handles
	delete scene;
	delete model;
	delete assetsManager;
	delete renderer;
	delete context;

	// destroy GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
