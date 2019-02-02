#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

int main(void)
{
	// init GLFW
	glfwInit();
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
