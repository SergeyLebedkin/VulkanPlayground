#include "vulkan_meshes.hpp"

// VulkanMesh_gui::~VulkanMesh_gui
VulkanMesh_gui::VulkanMesh_gui(
	VulkanDevice&                       device,
	VulkanMaterial*                     material,
	std::vector<VertexStruct_P4_C4_T2>& verts) :
	VulkanMesh_material(device, material)
{
	// create buffers
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(verts), &bufferVerteces);
	// write buffers
	vulkanBufferWrite(device, bufferVerteces, 0, VKT_VECTOR_DATA_SIZE(verts), verts.data());
	vertexCount = (uint32_t)verts.size();
}

// VulkanMesh_gui::~VulkanMesh_gui
VulkanMesh_gui::~VulkanMesh_gui()
{
	vulkanBufferDestroy(device, bufferVerteces);
}

// VulkanMesh_gui::draw
void VulkanMesh_gui::draw(VulkanCommandBuffer& commandBuffer)
{
	// prepare buffers
	VkDeviceSize offsets[] = { 0 };
	VkBuffer buffers[] = { bufferVerteces.buffer };

	// render
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 1, buffers, offsets);
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
};

//////////////////////////////////////////////////////////////////////////

// VulkanMesh_obj::VulkanMesh_obj
VulkanMesh_obj::VulkanMesh_obj(
	VulkanDevice&           device,
	VulkanMaterial*         material,
	std::vector<glm::vec3>& pos,
	std::vector<glm::vec2>& tex,
	std::vector<glm::vec3>& nrm) :
	VulkanMesh_material(device, material)
{
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
void VulkanMesh_obj::draw(VulkanCommandBuffer& commandBuffer)
{
	// prepare buffers
	VkDeviceSize offsets[] = { 0, 0, 0 };
	VkBuffer buffers[]{ bufferPos.buffer, bufferTex.buffer, bufferNrm.buffer };

	// render
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 3, buffers, offsets);
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
};

//////////////////////////////////////////////////////////////////////////

// VulkanMesh_lines::VulkanMesh_lines
VulkanMesh_lines::VulkanMesh_lines(
	VulkanDevice&           device,
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
	vertexCount = (uint32_t)pos.size();
}

// VulkanMesh_lines::~VulkanMesh_lines
VulkanMesh_lines::~VulkanMesh_lines()
{
	vulkanBufferDestroy(device, bufferCol);
	vulkanBufferDestroy(device, bufferPos);
}

// VulkanMesh_lines::draw(
void VulkanMesh_lines::draw(VulkanCommandBuffer& commandBuffer)
{
	// prepare buffers
	VkDeviceSize offsets[] = { 0, 0 };
	VkBuffer buffers[]{ bufferPos.buffer, bufferCol.buffer };

	// render
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 2, buffers, offsets);
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
};