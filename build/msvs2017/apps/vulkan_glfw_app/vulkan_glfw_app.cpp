#include <vktoolkit.hpp>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <tiny_obj_loader.h>
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable: 4996)
#include <stb_image.h>
#include <stb_image_write.h>
#pragma warning(pop)

// vertex structure
struct VertexStruct_P4_C4_T2 { float X, Y, Z, W; float R, G, B, A; float U, V; };

// vertex array
VertexStruct_P4_C4_T2 vertices[] = {
	{ +1.0f, -1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +0.0f, +1.0, /**/+1, +0 },
	{ +1.0f, +1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +0.0f, +1.0, /**/+1, +1 },
	{ -1.0f, -1.0f, +0.0f, +1.0, /**/+1.0f, +0.0f, +0.0f, +1.0, /**/+0, +0 },
	{ -1.0f, +1.0f, +0.0f, +1.0, /**/+1.0f, +1.0f, +0.0f, +1.0, /**/+0, +1 },
};

// index array
uint16_t indexes[] = { 0, 1, 2, 2, 1, 3 };

// loadImageFromFile
void loadImageFromFile(VulkanDevice& device, VulkanImage& image, const char* fileName)
{
	int width = 0, height = 0, channels = 0;
	stbi_uc* texData = stbi_load(fileName, &width, &height, &channels, 4);
	vulkanImageCreate(device, VK_FORMAT_R8G8B8A8_UNORM, width, height, 1, &image);
	vulkanImageWrite(device, image, 0, texData);
	vulkanImageBuildMipmaps(device, image);
	stbi_image_free(texData);
}

// loadMesh_obj
void loadMesh_obj(VulkanDevice& device, VulkanBuffer& bufferPos, VulkanBuffer& bufferTex, VulkanBuffer& bufferNrm, uint32_t& vertexCount, const char* fileName, const char* baseDir)
{
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warm, err;
	tinyobj::LoadObj(&attribs, &shapes, &materials, &warm, &err, fileName, baseDir, true);
	std::cout << err << std::endl;

	// allocate buffers
	std::vector<float> vectorPos{};
	std::vector<float> vectorNrm{};
	std::vector<float> vectorTex{};
	//for (const auto& shape : shapes)
	for (size_t i = 0; i < 1; i++)
	{
		auto& shape = shapes[i];

		vectorPos.clear();
		vectorNrm.clear();
		vectorTex.clear();
		vectorPos.reserve(shape.mesh.indices.size() * 3);
		vectorNrm.reserve(shape.mesh.indices.size() * 3);
		vectorTex.reserve(shape.mesh.indices.size() * 2);

		// create buffers
		for (tinyobj::index_t index : shape.mesh.indices)
		{
			// if vertex exists
			if (index.vertex_index >= 0)
			{
				vectorPos.push_back(attribs.vertices[3 * index.vertex_index + 0]);
				vectorPos.push_back(attribs.vertices[3 * index.vertex_index + 1]);
				vectorPos.push_back(attribs.vertices[3 * index.vertex_index + 2]);
			}

			// if normal exists
			if (index.normal_index >= 0)
			{
				vectorNrm.push_back(attribs.normals[3 * index.normal_index + 0]);
				vectorNrm.push_back(attribs.normals[3 * index.normal_index + 1]);
				vectorNrm.push_back(attribs.normals[3 * index.normal_index + 2]);
			}

			// if texCoords exists
			if (index.texcoord_index >= 0)
			{
				vectorTex.push_back(attribs.texcoords[2 * index.texcoord_index + 0]);
				vectorTex.push_back(attribs.texcoords[2 * index.texcoord_index + 1]);
			}
		}
		vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vectorPos.size() * sizeof(float), &bufferPos);
		vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vectorTex.size() * sizeof(float), &bufferTex);
		vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vectorNrm.size() * sizeof(float), &bufferNrm);
		vulkanBufferWrite(device, bufferPos, 0, vectorPos.size() * sizeof(float), vectorPos.data());
		vulkanBufferWrite(device, bufferTex, 0, vectorTex.size() * sizeof(float), vectorTex.data());
		vulkanBufferWrite(device, bufferNrm, 0, vectorNrm.size() * sizeof(float), vectorNrm.data());
		vertexCount = (uint32_t)vectorPos.size() / 3;
	}
}

// createPipeline_default
void createPipeline_default(VulkanDevice& device, VkRenderPass renderPass, VulkanPipeline& pipeline)
{
	// VkVertexInputBindingDescription
	VkVertexInputBindingDescription vertexBindingDescriptions[]
	{
	{ 0, sizeof(VertexStruct_P4_C4_T2), VK_VERTEX_INPUT_RATE_VERTEX },
	};

	// VkVertexInputAttributeDescription
	VkVertexInputAttributeDescription vertexInputAttributeDescriptions[]
	{
	{ 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT,  0 }, // position
	{ 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 16 }, // color
	{ 2, 0, VK_FORMAT_R32G32_SFLOAT      , 32 }, // texCoord
	};

	// VkDescriptorSetLayoutBinding
	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[]
	{
	{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, VK_NULL_HANDLE }, // texture
	{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1, VK_SHADER_STAGE_VERTEX_BIT  , VK_NULL_HANDLE }, // buffer
	};

	// VkPipelineColorBlendAttachmentState
	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentStates[]{
		{ // first attachments
			VK_FALSE,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		}
	};

	// create pipeline
	vulkanPipelineCreate(device, renderPass, 0,
		"shaders/base.vert.spv", "shaders/base.frag.spv",
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions), vertexBindingDescriptions,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions), vertexInputAttributeDescriptions,
		VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings), descriptorSetLayoutBindings,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates), pipelineColorBlendAttachmentStates,
		&pipeline);
}

// createPipeline_obj
void createPipeline_obj(VulkanDevice& device, VkRenderPass renderPass, VulkanPipeline& pipeline)
{
	// VkVertexInputBindingDescription
	VkVertexInputBindingDescription vertexBindingDescriptions[]
	{
	{ 0, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX },
	{ 1, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_VERTEX },
	{ 2, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX },
	};

	// VkVertexInputAttributeDescription
	VkVertexInputAttributeDescription vertexInputAttributeDescriptions[]
	{
	{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 }, // position
	{ 1, 1, VK_FORMAT_R32G32_SFLOAT,    0 }, // texCoord
	{ 2, 2, VK_FORMAT_R32G32B32_SFLOAT, 0 }, // normal
	};

	// VkDescriptorSetLayoutBinding
	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[]
	{
	{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, VK_NULL_HANDLE }, // texture
	{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1, VK_SHADER_STAGE_VERTEX_BIT  , VK_NULL_HANDLE }, // buffer
	};

	// VkPipelineColorBlendAttachmentState
	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentStates[]{
		{ // first attachments
			VK_FALSE,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		}
	};

	// create pipeline
	vulkanPipelineCreate(device, renderPass, 0,
		"shaders/obj.vert.spv", "shaders/obj.frag.spv",
		VKT_ARRAY_ELEMENTS_COUNT(vertexBindingDescriptions), vertexBindingDescriptions,
		VKT_ARRAY_ELEMENTS_COUNT(vertexInputAttributeDescriptions), vertexInputAttributeDescriptions,
		VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings), descriptorSetLayoutBindings,
		VKT_ARRAY_ELEMENTS_COUNT(pipelineColorBlendAttachmentStates), pipelineColorBlendAttachmentStates,
		&pipeline);
}

// main
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
	physicalDeviceFeatures.depthBounds = VK_TRUE;
	physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;

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

	// create pipeline
	VulkanPipeline  pipeline_obj{};
	VulkanPipeline  pipeline_default{};
	createPipeline_obj(device, swapchain.renderPass, pipeline_obj);
	createPipeline_default(device, swapchain.renderPass, pipeline_default);
	
	// create texture
	VulkanImage image{};
	loadImageFromFile(device, image, "textures/texture.png");

	// create sampler
	VulkanSampler sampler{};
	vulkanSamplerCreate(device, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_FALSE, &sampler);

	// create buffers
	VulkanBuffer bufferVertex{};
	VulkanBuffer bufferIndex{};
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, sizeof(vertices), &bufferVertex);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, sizeof(indexes), &bufferIndex);
	vulkanBufferWrite(device, bufferVertex, 0, sizeof(vertices), vertices);
	vulkanBufferWrite(device, bufferIndex, 0, sizeof(indexes), indexes);

	// create obj buffers
	uint32_t vertexCount = 0;
	VulkanBuffer bufferPos{};
	VulkanBuffer bufferTex{};
	VulkanBuffer bufferNrm{};
	loadMesh_obj(device, bufferPos, bufferTex, bufferNrm, vertexCount, "models/tea.obj", "models");

	// matrices
	float aspect = (float)swapchain.surfaceCapabilities.currentExtent.width / swapchain.surfaceCapabilities.currentExtent.height;
	glm::mat4 matModl = glm::scale(glm::mat4(1.0f), glm::vec3(0.125f, 0.125f, 0.125f));
	glm::mat4 matView = glm::lookAt(
		glm::vec3(0.0f, 4.0f, 7.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 matProj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.f);

	// buffer matrices
	VulkanBuffer bufferMatrices{};
	vulkanBufferCreate(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(glm::mat4) * 3, &bufferMatrices);
 	vulkanBufferWrite(device, bufferMatrices, sizeof(glm::mat4) * 0, sizeof(glm::mat4), &matModl);
 	vulkanBufferWrite(device, bufferMatrices, sizeof(glm::mat4) * 1, sizeof(glm::mat4), &matView);
 	vulkanBufferWrite(device, bufferMatrices, sizeof(glm::mat4) * 2, sizeof(glm::mat4), &matProj);

	// main loop
	while (!glfwWindowShouldClose(window))
	{
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

		// draw obj
		VkBuffer buffers[]{ bufferPos.buffer, bufferTex.buffer, bufferNrm.buffer };
		vulkanPipelineBindImage(device, pipeline_obj, image, sampler, 0);
		vulkanPipelineBindBufferUniform(device, pipeline_obj, bufferMatrices, 1);
		vkCmdBindPipeline(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_obj.pipeline);
		vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_obj.pipelineLayout, 0, 1, &pipeline_obj.descriptorSet, 0, VK_NULL_HANDLE);
		vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 3, buffers, offsets);
		vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);

// 		// draw simple quad
// 		vulkanPipelineBindImage(device, pipeline_default, image, sampler, 0);
// 		vkCmdBindPipeline(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_default.pipeline);
// 		vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_default.pipelineLayout, 0, 1, &pipeline_default.descriptorSet, 0, VK_NULL_HANDLE);
// 		vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 1, &bufferVertex.buffer, offsets);
// 		vkCmdBindIndexBuffer(commandBuffer.commandBuffer, bufferIndex.buffer, 0, VK_INDEX_TYPE_UINT16);
// 		vkCmdDrawIndexed(commandBuffer.commandBuffer, VKT_ARRAY_ELEMENTS_COUNT(indexes), 1, 0, 0, 0);
		
		// END RENDER
		vkCmdEndRenderPass(commandBuffer.commandBuffer);

		// vkEndCommandBuffer
		VKT_CHECK(vkEndCommandBuffer(commandBuffer.commandBuffer));
		
		vulkanQueueSubmit(device, commandBuffer, &presentSemaphore, &renderSemaphore);
		vulkanSwapchainEndFrame(device, swapchain, renderSemaphore, frameIndex);
		glfwPollEvents();
	}

	// destroy vulkan
	vulkanBufferDestroy(device, bufferMatrices);
	vulkanBufferDestroy(device, bufferNrm);
	vulkanBufferDestroy(device, bufferTex);
	vulkanBufferDestroy(device, bufferPos);
	vulkanBufferDestroy(device, bufferIndex);
	vulkanBufferDestroy(device, bufferVertex);
	vulkanSamplerDestroy(device, sampler);
	vulkanImageDestroy(device, image);
	vulkanPipelineDestroy(device, pipeline_default);
	vulkanPipelineDestroy(device, pipeline_obj);
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
