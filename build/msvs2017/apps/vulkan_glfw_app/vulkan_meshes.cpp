#include "vulkan_meshes.hpp"

// VulkanMesh::VulkanMesh
VulkanMesh::VulkanMesh(
	VulkanDevice&              device,
	VulkanDescriptorSetLayout& descriptorSetLayout) :
	device(device),
	descriptorSetLayout(descriptorSetLayout)
{
	vulkanBufferCreate(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 3 * sizeof(glm::mat4), &bufferMVP);
	vulkanDescriptorSetCreate(device, descriptorSetLayout, &descriptorSet);
}

// VulkanMesh::~VulkanMesh
VulkanMesh::~VulkanMesh()
{
	vulkanDescriptorSetDestroy(device, descriptorSet);
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

// VulkanMesh_gui::~VulkanMesh_gui
VulkanMesh_gui::VulkanMesh_gui(
		VulkanDevice&                       device,
		VulkanDescriptorSetLayout&          descriptorSetLayout,
		std::vector<VertexStruct_P4_C4_T2>& verts) :
	VulkanMesh(device, descriptorSetLayout)
{
	// update descriptor set
	vulkanDescriptorSetUpdateBufferUniform(device, descriptorSet, bufferMVP, 1);
	// create buffers
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(verts), &bufferVerts);
	// write buffers
	vulkanBufferWrite(device, bufferVerts, 0, VKT_VECTOR_DATA_SIZE(verts), verts.data());
	vertexCount = (uint32_t)verts.size();
}

// VulkanMesh_gui::~VulkanMesh_gui
VulkanMesh_gui::~VulkanMesh_gui()
{
	vulkanBufferDestroy(device, bufferVerts);
}

// VulkanMesh_gui::draw
void VulkanMesh_gui::draw(
	VulkanPipeline&      pipeline,
	VulkanCommandBuffer& commandBuffer,
	glm::mat4&           matProj,
	glm::mat4&           matView,
	glm::mat4&           matModl)
{
	// prepare buffers
	VkDeviceSize offsets[] = { 0 };
	VkBuffer buffers[] = { bufferVerts.buffer };

	// update MVP
	struct { glm::mat4 matM; glm::mat4 matV; glm::mat4 matP; } matrixes{ matModl, matView, matProj };
	vulkanBufferWrite(device, bufferMVP, 0, sizeof(matrixes), &matrixes);

	// render
	vkCmdBindPipeline(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
	vkCmdBindDescriptorSets(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &descriptorSet.descriptorSet, 0, VK_NULL_HANDLE);
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 1, buffers, offsets);
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
};

// VulkanMesh_obj::VulkanMesh_obj
VulkanMesh_obj::VulkanMesh_obj(
	VulkanDevice&              device,
	VulkanDescriptorSetLayout& descriptorSetLayout,
	std::vector<glm::vec3>&    pos,
	std::vector<glm::vec2>&    tex,
	std::vector<glm::vec3>&    nrm) :
	VulkanMesh(device, descriptorSetLayout)
{
	// update descriptor set
	vulkanDescriptorSetUpdateBufferUniform(device, descriptorSet, bufferMVP, 1);
	// create buffers
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(pos), &bufferPos);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(tex), &bufferTex);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(nrm), &bufferNrm);
	// write buffers
	vulkanBufferWrite(device, bufferPos, 0, VKT_VECTOR_DATA_SIZE(pos), pos.data());
	vulkanBufferWrite(device, bufferTex, 0, VKT_VECTOR_DATA_SIZE(tex), tex.data());
	vulkanBufferWrite(device, bufferNrm, 0, VKT_VECTOR_DATA_SIZE(nrm), nrm.data());
	vertexCount = (uint32_t)pos.size();
}

// VulkanMesh_obj::~VulkanMesh_obj
VulkanMesh_obj::~VulkanMesh_obj()
{	
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
	VulkanDevice&              device,
	VulkanDescriptorSetLayout& descriptorSetLayout,
	std::vector<glm::vec3>&    pos,
	std::vector<glm::vec4>&    col) :
	VulkanMesh(device, descriptorSetLayout)
{
	// update descriptor set
	vulkanDescriptorSetUpdateBufferUniform(device, descriptorSet, bufferMVP, 0);
	// create buffers
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(pos), &bufferPos);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(col), &bufferCol);
	// write buffers
	vulkanBufferWrite(device, bufferPos, 0, VKT_VECTOR_DATA_SIZE(pos), pos.data());
	vulkanBufferWrite(device, bufferCol, 0, VKT_VECTOR_DATA_SIZE(col), col.data());
	vertexCount = (uint32_t)pos.size();
}

// VulkanMesh_lines::~VulkanMesh_lines
VulkanMesh_lines::~VulkanMesh_lines()
{
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