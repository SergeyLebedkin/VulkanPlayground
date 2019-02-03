#include <vktoolkit.hpp>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
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
	VulkanInstance  instance{};
	VulkanSurface   surface{};
	VulkanDevice    device{};
	VulkanSwapchain swapchain{};
	VulkanSemaphore renderFinishedSemaphore{};
	VulkanSemaphore imageAvailableSemaphore{};
	vulkanInstanceCreate(enabledInstanceLayerNames, enabledInstanceExtensionNames, &instance);
	glfwCreateWindowSurface(instance.instance, window, NULL, &surface.surface);
	vulkanDeviceCreate(&instance, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, physicalDeviceFeatures, enabledDeviceExtensionNames, &device);
	vulkanSwapchainCreate(&device, &surface, &swapchain);
	vulkanSemaphoreCreate(&device, &renderFinishedSemaphore);
	vulkanSemaphoreCreate(&device, &imageAvailableSemaphore);

	// main loop
	while (!glfwWindowShouldClose(window))
	{
		uint32_t frameIndex = 0;
		VkFramebuffer framebuffer = vulkanSwapchainBeginFrame(&device, &swapchain, &imageAvailableSemaphore, frameIndex);
		vulkanSwapchainEndFrame(&device, &swapchain, &imageAvailableSemaphore, frameIndex);
		glfwPollEvents();
	}

	// destroy vulkan
	vulkanSemaphoreDestroy(&device, &imageAvailableSemaphore);
	vulkanSemaphoreDestroy(&device, &renderFinishedSemaphore);
	vulkanSwapchainDestroy(&device, &swapchain);
	vkDestroySurfaceKHR(instance.instance, surface.surface, NULL);
	vulkanDeviceDestroy(&device);
	vulkanInstanceDestroy(&instance);

	// destroy GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
