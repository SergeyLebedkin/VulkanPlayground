//#include <vulkan/vulkan.h>
#include <VmaUsage.h>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>

int main(void)
{
	// init GLFW
	glfwInit();
	if (!glfwVulkanSupported()) assert(0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);

	// main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	// destroy GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
}
