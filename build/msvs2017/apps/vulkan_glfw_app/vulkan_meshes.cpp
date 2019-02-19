#include "vulkan_meshes.hpp"

// VulkanMesh_obj::VulkanMesh_obj
VulkanMesh_obj::VulkanMesh_obj(VulkanDevice& device) :
	device(device)
{
}

// VulkanMesh_obj::~VulkanMesh_obj
VulkanMesh_obj::~VulkanMesh_obj()
{
	VulkanDescriptorSetDestroy(device, descriptorSet);
	vulkanBufferDestroy(device, bufferMVP);
	vulkanBufferDestroy(device, bufferNrm);
	vulkanBufferDestroy(device, bufferTex);
	vulkanBufferDestroy(device, bufferPos);
}

// VulkanMesh_obj::draw
void VulkanMesh_obj::draw(
	VulkanPipeline&      pipeline,
	VulkanCommandBuffer& commandBuffer,
	glm::mat4&           matProj,
	glm::mat4&           matView,
	glm::mat4&           matModl)
{
	// prepare buffers
	VkDeviceSize offsets[] = { 0, 0, 0 };
	VkBuffer buffers[]{ bufferPos.buffer, bufferTex.buffer, bufferNrm.buffer };

	struct { glm::mat4 matM; glm::mat4 matV; glm::mat4 matP; } matrixes{ matModl, matView, matProj };
	
	// update data
	//if (!matwrite)
	{
		vulkanBufferWrite(device, bufferMVP, 0, sizeof(matrixes), &matrixes);
		//vulkanBufferWrite(device, bufferMVP, sizeof(glm::mat4) * 0, sizeof(glm::mat4), &matModl);
		//vulkanBufferWrite(device, bufferMVP, sizeof(glm::mat4) * 1, sizeof(glm::mat4), &matView);
		//vulkanBufferWrite(device, bufferMVP, sizeof(glm::mat4) * 2, sizeof(glm::mat4), &matProj);
		matwrite = true;
	}

	// render
	vkCmdBindPipeline(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 3, buffers, offsets);
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
};