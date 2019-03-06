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
	//VulkanModel* modelRock = assetsManager->createModelByMeshGroupName("models/train/train.obj");
	//VulkanModel* modelRock = assetsManager->createModelByMeshGroupName("models/rock/rock.obj");
	VulkanModel* model = assetsManager->createModelByMeshGroupName("models/tea/tea.obj");


	delete model;
	delete assetsManager;
	delete renderer;
	delete context;

	// destroy GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
