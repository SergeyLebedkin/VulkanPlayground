#include "vulkan_meshes.hpp"

// VulkanMesh::VulkanMesh
VulkanMesh::VulkanMesh(VulkanDevice& device) : device(device)
{
	vulkanBufferCreate(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 3 * sizeof(glm::mat4), &bufferMVP);
}

// VulkanMesh::~VulkanMesh
VulkanMesh::~VulkanMesh()
{
	vulkanBufferDestroy(device, bufferMVP);
}

// VulkanMesh::setImage
void VulkanMesh::setImage(
	VulkanImage&   image,
	VulkanSampler& sampler,
	uint32_t       binding)
{
	vulkanDescriptorSetUpdateImage(device, descriptorSet, image, sampler, binding);
}

// VulkanMesh_obj::VulkanMesh_obj
VulkanMesh_obj::VulkanMesh_obj(
	VulkanDevice&           device,
	VulkanPipeline&         pipeline,
	std::vector<glm::vec3>& pos,
	std::vector<glm::vec2>& tex,
	std::vector<glm::vec3>& nrm) :
	VulkanMesh(device)
{
	// create buffers
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(pos), &bufferPos);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(tex), &bufferTex);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(nrm), &bufferNrm);
	// write buffers
	vulkanBufferWrite(device, bufferPos, 0, VKT_VECTOR_DATA_SIZE(pos), pos.data());
	vulkanBufferWrite(device, bufferTex, 0, VKT_VECTOR_DATA_SIZE(tex), tex.data());
	vulkanBufferWrite(device, bufferNrm, 0, VKT_VECTOR_DATA_SIZE(nrm), nrm.data());
	// create descriptor set
	VulkanDescriptorSetCreate(device, pipeline, VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_T1_U1), descriptorSetLayoutBindings_T1_U1, &descriptorSet);
	vulkanDescriptorSetUpdateBufferUniform(device, descriptorSet, bufferMVP, 1);
	vertexCount = (uint32_t)pos.size();
}

// VulkanMesh_obj::~VulkanMesh_obj
VulkanMesh_obj::~VulkanMesh_obj()
{
	VulkanDescriptorSetDestroy(device, descriptorSet);
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

	// update MVP
	struct { glm::mat4 matM; glm::mat4 matV; glm::mat4 matP; } matrixes{ matModl, matView, matProj };
	vulkanBufferWrite(device, bufferMVP, 0, sizeof(matrixes), &matrixes);

	// render
	vkCmdBindPipeline(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 3, buffers, offsets);
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
};

// VulkanMesh_lines::VulkanMesh_lines
VulkanMesh_lines::VulkanMesh_lines(
	VulkanDevice&           device,
	VulkanPipeline&         pipeline,
	std::vector<glm::vec3>& pos,
	std::vector<glm::vec4>& col) :
	VulkanMesh(device)
{
	// create buffers
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(pos), &bufferPos);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(col), &bufferCol);
	// write buffers
	vulkanBufferWrite(device, bufferPos, 0, VKT_VECTOR_DATA_SIZE(pos), pos.data());
	vulkanBufferWrite(device, bufferCol, 0, VKT_VECTOR_DATA_SIZE(col), col.data());
	// create descriptor set
	VulkanDescriptorSetCreate(device, pipeline, VKT_ARRAY_ELEMENTS_COUNT(descriptorSetLayoutBindings_U1), descriptorSetLayoutBindings_U1, &descriptorSet);
	vulkanDescriptorSetUpdateBufferUniform(device, descriptorSet, bufferMVP, 0);
	vertexCount = (uint32_t)pos.size();
}

// VulkanMesh_lines::~VulkanMesh_lines
VulkanMesh_lines::~VulkanMesh_lines()
{
	VulkanDescriptorSetDestroy(device, descriptorSet);
	vulkanBufferDestroy(device, bufferCol);
	vulkanBufferDestroy(device, bufferPos);
}

// VulkanMesh_lines::draw(
void VulkanMesh_lines::draw(
	VulkanPipeline&      pipeline,
	VulkanCommandBuffer& commandBuffer,
	glm::mat4&           matProj,
	glm::mat4&           matView,
	glm::mat4&           matModl)
{
	// prepare buffers
	VkDeviceSize offsets[] = { 0, 0 };
	VkBuffer buffers[]{ bufferPos.buffer, bufferCol.buffer };

	// update MVP
	struct { glm::mat4 matM; glm::mat4 matV; glm::mat4 matP; } matrixes{ matModl, matView, matProj };
	vulkanBufferWrite(device, bufferMVP, 0, sizeof(matrixes), &matrixes);

	// render
	vkCmdBindPipeline(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 2, buffers, offsets);
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
};