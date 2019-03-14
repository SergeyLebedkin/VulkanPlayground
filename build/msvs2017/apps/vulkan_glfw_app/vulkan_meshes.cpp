#include "vulkan_meshes.hpp"
#include "vulkan_context.hpp"

// VulkanMeshMatObj::VulkanMeshMatObj
VulkanMeshMatObj::VulkanMeshMatObj(
	VulkanContext&          context,
	std::vector<glm::vec4>& pos,
	std::vector<glm::vec2>& tex,
	std::vector<glm::vec3>& nrm) :
	VulkanMeshMaterial(context)
{
	// create buffers
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(pos), &bufferPos);
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(tex), &bufferTex);
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(nrm), &bufferNrm);
	// write buffers
	vulkanBufferWrite(context.device, bufferPos, 0, VKT_VECTOR_DATA_SIZE(pos), pos.data());
	vulkanBufferWrite(context.device, bufferTex, 0, VKT_VECTOR_DATA_SIZE(tex), tex.data());
	vulkanBufferWrite(context.device, bufferNrm, 0, VKT_VECTOR_DATA_SIZE(nrm), nrm.data());
	vertexCount = (uint32_t)pos.size();
	// setup buffer handles and offsets
	bufferHandles = { bufferPos.buffer, bufferTex.buffer, bufferNrm.buffer };
	bufferOffsets = { 0, 0, 0 };
}

// VulkanMeshMatObj::~VulkanMeshMatObj
VulkanMeshMatObj::~VulkanMeshMatObj() {
	// destroy buffers
	vulkanBufferDestroy(context.device, bufferNrm);
	vulkanBufferDestroy(context.device, bufferTex);
	vulkanBufferDestroy(context.device, bufferPos);
}

// VulkanMeshMatObj::draw
void VulkanMeshMatObj::draw(VulkanCommandBuffer& commandBuffer) {
	// bind and draw
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, (uint32_t)bufferHandles.size(), bufferHandles.data(), bufferOffsets.data());
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
}

//////////////////////////////////////////////////////////////////////////

// VulkanMeshMatObjIndexed::VulkanMeshMatObjIndexed
VulkanMeshMatObjIndexed::VulkanMeshMatObjIndexed(
	VulkanContext&          context,
	std::vector<glm::vec4>& pos,
	std::vector<glm::vec2>& tex,
	std::vector<glm::vec3>& nrm,
	std::vector<uint32_t>&  ind) :
	VulkanMeshMatObj(context, pos, tex, nrm)
{
	// create index buffer
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(ind), &bufferInd);
	// write index buffers
	vulkanBufferWrite(context.device, bufferInd, 0, VKT_VECTOR_DATA_SIZE(ind), ind.data());
	indexCount = (uint32_t)ind.size();
}

// VulkanMeshMatObjIndexed::~VulkanMeshMatObjIndexed
VulkanMeshMatObjIndexed::~VulkanMeshMatObjIndexed()
{
	// destroy buffers
	vulkanBufferDestroy(context.device, bufferInd);
}

// draw
void VulkanMeshMatObjIndexed::draw(VulkanCommandBuffer& commandBuffer) {
	// bind and draw
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, (uint32_t)bufferHandles.size(), bufferHandles.data(), bufferOffsets.data());
	vkCmdBindIndexBuffer(commandBuffer.commandBuffer, bufferInd.buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(commandBuffer.commandBuffer, indexCount, 1, 0, 0, 0);
}

//////////////////////////////////////////////////////////////////////////

// VulkanMeshMatObjTBN::VulkanMeshMatObjTBN
VulkanMeshMatObjTBN::VulkanMeshMatObjTBN(
	VulkanContext&          context,
	std::vector<glm::vec4>& pos,
	std::vector<glm::vec2>& tex,
	std::vector<glm::vec3>& nrm,
	std::vector<glm::vec3>& tng,
	std::vector<glm::vec3>& bnm) : 
	VulkanMeshMatObj(context, pos, tex, nrm)
{
	// create buffers
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(tng), &bufferTng);
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(bnm), &bufferBnm);
	// write buffers
	vulkanBufferWrite(context.device, bufferTng, 0, VKT_VECTOR_DATA_SIZE(tng), tng.data());
	vulkanBufferWrite(context.device, bufferBnm, 0, VKT_VECTOR_DATA_SIZE(bnm), bnm.data());
	// setup buffer handles and offsets
	bufferHandlesTB = { bufferTng.buffer, bufferBnm.buffer };
	bufferOffsetsTB = { 0, 0 };
}

// VulkanMeshMatObjTBN::~VulkanMeshMatObjTBN
VulkanMeshMatObjTBN::~VulkanMeshMatObjTBN() {
	// destroy buffers
	vulkanBufferDestroy(context.device, bufferBnm);
	vulkanBufferDestroy(context.device, bufferTng);
}

// VulkanMeshMatObjTBN::draw
void VulkanMeshMatObjTBN::draw(VulkanCommandBuffer& commandBuffer) {
	// bind and draw
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, (uint32_t)bufferHandles.size(), bufferHandles.data(), bufferOffsets.data());
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 3, (uint32_t)bufferHandlesTB.size(), bufferHandlesTB.data(), bufferOffsetsTB.data());
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
}

//////////////////////////////////////////////////////////////////////////

// VulkanMeshMatObjTBNIndexed::VulkanMeshMatObjTBNIndexed
VulkanMeshMatObjTBNIndexed::VulkanMeshMatObjTBNIndexed(
	VulkanContext&          context,
	std::vector<glm::vec4>& pos,
	std::vector<glm::vec2>& tex,
	std::vector<glm::vec3>& nrm,
	std::vector<glm::vec3>& tng,
	std::vector<glm::vec3>& bnm,
	std::vector<uint32_t>&  ind) : 
	VulkanMeshMatObjTBN(context, pos, tex, nrm, tng, bnm)
{
	// create index buffer
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(ind), &bufferInd);
	// write index buffers
	vulkanBufferWrite(context.device, bufferInd, 0, VKT_VECTOR_DATA_SIZE(ind), ind.data());
	indexCount = (uint32_t)ind.size();
}

// VulkanMeshMatObjTBNIndexed::~VulkanMeshMatObjTBNIndexed
VulkanMeshMatObjTBNIndexed::~VulkanMeshMatObjTBNIndexed() {
	// destroy buffers
	vulkanBufferDestroy(context.device, bufferInd);
}

// VulkanMeshMatObjTBNIndexed::draw
void VulkanMeshMatObjTBNIndexed::draw(VulkanCommandBuffer& commandBuffer) {
	// bind and draw
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, (uint32_t)bufferHandles.size(), bufferHandles.data(), bufferOffsets.data());
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 3, (uint32_t)bufferHandlesTB.size(), bufferHandlesTB.data(), bufferOffsetsTB.data());
	vkCmdBindIndexBuffer(commandBuffer.commandBuffer, bufferInd.buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(commandBuffer.commandBuffer, indexCount, 1, 0, 0, 0);
}