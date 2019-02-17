#include "vulkan_meshes.hpp"

// VulkanMesh_obj::VulkanMesh_obj
VulkanMesh_obj::VulkanMesh_obj(VulkanDevice& device) :
	device(device)
{
}

// VulkanMesh_obj::~VulkanMesh_obj
VulkanMesh_obj::~VulkanMesh_obj()
{
	vulkanBufferDestroy(device, bufferNrm);
	vulkanBufferDestroy(device, bufferTex);
	vulkanBufferDestroy(device, bufferPos);
}

// VulkanMesh_obj::draw
void VulkanMesh_obj::draw(VulkanPipeline& pipeline, VulkanCommandBuffer& commandBuffer)
{
	// prepare buffers
	VkDeviceSize offsets[] = { 0, 0, 0 };
	VkBuffer buffers[]{ bufferPos.buffer, bufferTex.buffer, bufferNrm.buffer };

	// apply image and sampler
	if (image && sampler)
		vulkanPipelineBindImage(device, pipeline, *image, *sampler, 0);

	// start render
	vkCmdBindPipeline(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &pipeline.descriptorSet, 0, VK_NULL_HANDLE);
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 3, buffers, offsets);
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
};