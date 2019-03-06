#include "vulkan_meshes.hpp"
#include "vulkan_context.hpp"

// VulkanMesh_material
void VulkanMesh_material::draw(VulkanCommandBuffer& commandBuffer)
{
	// bind material if exists
	if (material)
		material->bind(commandBuffer);
}

//////////////////////////////////////////////////////////////////////////

// VulkanMesh_gui::~VulkanMesh_gui
VulkanMesh_gui::VulkanMesh_gui(
	VulkanContext&                      context,
	VulkanMaterial*                     material,
	std::vector<VertexStruct_P4_C4_T2>& verts,
	VkPrimitiveTopology                 primitiveTopology) :
	VulkanMesh_material(context, material), primitiveTopology(primitiveTopology)
{
	// create buffers
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(verts), &bufferVerteces);
	// write buffers
	vulkanBufferWrite(context.device, bufferVerteces, 0, VKT_VECTOR_DATA_SIZE(verts), verts.data());
	vertexCount = (uint32_t)verts.size();
	// setup buffer handles and offsets
	bufferHandles = { bufferVerteces.buffer };
	bufferOffsets = { 0 };
}

// VulkanMesh_gui::~VulkanMesh_gui
VulkanMesh_gui::~VulkanMesh_gui()
{
	// destroy buffers
	vulkanBufferDestroy(context.device, bufferVerteces);
}

// VulkanMesh_gui::draw
void VulkanMesh_gui::draw(VulkanCommandBuffer& commandBuffer)
{
	// bind material
	VulkanMesh_material::draw(commandBuffer);

	// render
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 1, bufferHandles.data(), bufferOffsets.data());
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
};

//////////////////////////////////////////////////////////////////////////

// VulkanMesh_indexed::VulkanMesh_indexed
VulkanMesh_indexed::VulkanMesh_indexed(
	VulkanContext&         context,
	VulkanMaterial*        material,
	std::vector<uint32_t>& indexes) :
	VulkanMesh_material(context, material)
{
	// create buffers
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(indexes), &bufferIndexes);
	// write buffers
	vulkanBufferWrite(context.device, bufferIndexes, 0, VKT_VECTOR_DATA_SIZE(indexes), indexes.data());
	indexCount = (uint32_t)indexes.size();
}

// VulkanMesh_indexed::~VulkanMesh_indexed
VulkanMesh_indexed::~VulkanMesh_indexed()
{
	// destroy buffers
	vulkanBufferDestroy(context.device, bufferIndexes);
}

// VulkanMesh_indexed::draw
void VulkanMesh_indexed::draw(VulkanCommandBuffer & commandBuffer)
{
	// bind material
	VulkanMesh_material::draw(commandBuffer);

	// bind index buffer
	vkCmdBindIndexBuffer(commandBuffer.commandBuffer, bufferIndexes.buffer, 0, VK_INDEX_TYPE_UINT32);
}

//////////////////////////////////////////////////////////////////////////

// VulkanMesh_indexed_obj::VulkanMesh_indexed_obj
VulkanMesh_indexed_obj::VulkanMesh_indexed_obj(
	VulkanContext&          context,
	VulkanMaterial*         material,
	std::vector<uint32_t>&  indexes,
	std::vector<glm::vec3>& pos,
	std::vector<glm::vec2>& tex,
	std::vector<glm::vec3>& nrm,
	VkPrimitiveTopology     primitiveTopology) :
	VulkanMesh_indexed(context, material, indexes),
	primitiveTopology(primitiveTopology)
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

// VulkanMesh_indexed_obj::~VulkanMesh_indexed_obj
VulkanMesh_indexed_obj::~VulkanMesh_indexed_obj()
{
	// destroy buffers
	vulkanBufferDestroy(context.device, bufferNrm);
	vulkanBufferDestroy(context.device, bufferTex);
	vulkanBufferDestroy(context.device, bufferPos);
}

// VulkanMesh_obj::draw
void VulkanMesh_indexed_obj::draw(VulkanCommandBuffer& commandBuffer)
{
	// bind material
	VulkanMesh_indexed::draw(commandBuffer);

	// render
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 3, bufferHandles.data(), bufferOffsets.data());
	vkCmdDrawIndexed(commandBuffer.commandBuffer, indexCount, 1, 0, 0, 0);
};

//////////////////////////////////////////////////////////////////////////

// VulkanMesh_obj::VulkanMesh_obj
VulkanMesh_obj::VulkanMesh_obj(
	VulkanContext&          context,
	VulkanMaterial*         material,
	std::vector<glm::vec3>& pos,
	std::vector<glm::vec2>& tex,
	std::vector<glm::vec3>& nrm,
	VkPrimitiveTopology     primitiveTopology) :
	VulkanMesh_material(context, material),
	primitiveTopology(primitiveTopology)
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

// VulkanMesh_obj::~VulkanMesh_obj
VulkanMesh_obj::~VulkanMesh_obj()
{	
	// destroy buffers
	vulkanBufferDestroy(context.device, bufferNrm);
	vulkanBufferDestroy(context.device, bufferTex);
	vulkanBufferDestroy(context.device, bufferPos);
}

// VulkanMesh_obj::draw
void VulkanMesh_obj::draw(VulkanCommandBuffer& commandBuffer)
{
	// bind material
	VulkanMesh_material::draw(commandBuffer);

	// render
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 3, bufferHandles.data(), bufferOffsets.data());
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
};

//////////////////////////////////////////////////////////////////////////

// VulkanMesh_debug::VulkanMesh_debug
VulkanMesh_debug::VulkanMesh_debug(
	VulkanContext&          context,
	std::vector<glm::vec3>& pos,
	std::vector<glm::vec4>& col,
	VkPrimitiveTopology     primitiveTopology) :
	VulkanMesh(context), primitiveTopology(primitiveTopology)
{
	// create buffers
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(pos), &bufferPos);
	vulkanBufferCreate(context.device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(col), &bufferCol);
	// write buffers
	vulkanBufferWrite(context.device, bufferPos, 0, VKT_VECTOR_DATA_SIZE(pos), pos.data());
	vulkanBufferWrite(context.device, bufferCol, 0, VKT_VECTOR_DATA_SIZE(col), col.data());
	vertexCount = (uint32_t)pos.size();
	// setup buffer handles and offsets
	bufferHandles = { bufferPos.buffer, bufferCol.buffer };
	bufferOffsets = { 0, 0 };
}

// VulkanMesh_debug::~VulkanMesh_debug
VulkanMesh_debug::~VulkanMesh_debug()
{
	// destroy buffers
	vulkanBufferDestroy(context.device, bufferCol);
	vulkanBufferDestroy(context.device, bufferPos);
}

// VulkanMesh_debug::draw(
void VulkanMesh_debug::draw(VulkanCommandBuffer& commandBuffer)
{
	// render
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 2, bufferHandles.data(), bufferOffsets.data());
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
};
