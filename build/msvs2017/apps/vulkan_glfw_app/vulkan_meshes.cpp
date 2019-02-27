#include "vulkan_meshes.hpp"

// VulkanMesh::draw
void VulkanMesh::draw(VulkanCommandBuffer& commandBuffer)
{
	// bind pipeline
	vkCmdBindPipeline(commandBuffer.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
}

//////////////////////////////////////////////////////////////////////////

// VulkanMesh_material
void VulkanMesh_material::draw(VulkanCommandBuffer& commandBuffer)
{
	// bind pipeline
	VulkanMesh::draw(commandBuffer);

	// bind material if exists
	if (material)
		material->bind(commandBuffer);
}

//////////////////////////////////////////////////////////////////////////

// VulkanMesh_gui::~VulkanMesh_gui
VulkanMesh_gui::VulkanMesh_gui(
	VulkanDevice&                       device,
	VulkanPipeline&                     pipeline,
	VulkanMaterial*                     material,
	std::vector<VertexStruct_P4_C4_T2>& verts) :
	VulkanMesh_material(device, pipeline, material)
{
	// create buffers
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(verts), &bufferVerteces);
	// write buffers
	vulkanBufferWrite(device, bufferVerteces, 0, VKT_VECTOR_DATA_SIZE(verts), verts.data());
	vertexCount = (uint32_t)verts.size();
	// setup buffer handles and offsets
	bufferHandles = { bufferVerteces.buffer };
	bufferOffsets = { 0 };
}

// VulkanMesh_gui::~VulkanMesh_gui
VulkanMesh_gui::~VulkanMesh_gui()
{
	// destroy buffers
	vulkanBufferDestroy(device, bufferVerteces);
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
	VulkanDevice&          device,
	VulkanPipeline&        pipeline,
	VulkanMaterial*        material,
	std::vector<uint32_t>& indexes) :
	VulkanMesh_material(device, pipeline, material)
{
	// create buffers
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(indexes), &bufferIndexes);
	// write buffers
	vulkanBufferWrite(device, bufferIndexes, 0, VKT_VECTOR_DATA_SIZE(indexes), indexes.data());
	indexCount = (uint32_t)indexes.size();
}

// VulkanMesh_indexed::~VulkanMesh_indexed
VulkanMesh_indexed::~VulkanMesh_indexed()
{
	// destroy buffers
	vulkanBufferDestroy(device, bufferIndexes);
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
	VulkanDevice&           device,
	VulkanPipeline&         pipeline,
	VulkanMaterial*         material,
	std::vector<uint32_t>&  indexes,
	std::vector<glm::vec3>& pos,
	std::vector<glm::vec2>& tex,
	std::vector<glm::vec3>& nrm) :
	VulkanMesh_indexed(device, pipeline, material, indexes)
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
	// setup buffer handles and offsets
	bufferHandles = { bufferPos.buffer, bufferTex.buffer, bufferNrm.buffer };
	bufferOffsets = { 0, 0, 0 };
}

// VulkanMesh_indexed_obj::~VulkanMesh_indexed_obj
VulkanMesh_indexed_obj::~VulkanMesh_indexed_obj()
{
	// destroy buffers
	vulkanBufferDestroy(device, bufferNrm);
	vulkanBufferDestroy(device, bufferTex);
	vulkanBufferDestroy(device, bufferPos);
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
	VulkanDevice&           device,
	VulkanPipeline&         pipeline,
	VulkanMaterial*         material,
	std::vector<glm::vec3>& pos,
	std::vector<glm::vec2>& tex,
	std::vector<glm::vec3>& nrm) :
	VulkanMesh_material(device, pipeline, material)
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
	// setup buffer handles and offsets
	bufferHandles = { bufferPos.buffer, bufferTex.buffer, bufferNrm.buffer };
	bufferOffsets = { 0, 0, 0 };
}

// VulkanMesh_obj::~VulkanMesh_obj
VulkanMesh_obj::~VulkanMesh_obj()
{	
	// destroy buffers
	vulkanBufferDestroy(device, bufferNrm);
	vulkanBufferDestroy(device, bufferTex);
	vulkanBufferDestroy(device, bufferPos);
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

// VulkanMesh_lines::VulkanMesh_lines
VulkanMesh_lines::VulkanMesh_lines(
	VulkanDevice&           device,
	VulkanPipeline&         pipeline,
	std::vector<glm::vec3>& pos,
	std::vector<glm::vec4>& col) :
	VulkanMesh(device, pipeline)
{
	// create buffers
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(pos), &bufferPos);
	vulkanBufferCreate(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VKT_VECTOR_DATA_SIZE(col), &bufferCol);
	// write buffers
	vulkanBufferWrite(device, bufferPos, 0, VKT_VECTOR_DATA_SIZE(pos), pos.data());
	vulkanBufferWrite(device, bufferCol, 0, VKT_VECTOR_DATA_SIZE(col), col.data());
	vertexCount = (uint32_t)pos.size();
	// setup buffer handles and offsets
	bufferHandles = { bufferPos.buffer, bufferCol.buffer };
	bufferOffsets = { 0, 0 };
}

// VulkanMesh_lines::~VulkanMesh_lines
VulkanMesh_lines::~VulkanMesh_lines()
{
	// destroy buffers
	vulkanBufferDestroy(device, bufferCol);
	vulkanBufferDestroy(device, bufferPos);
}

// VulkanMesh_lines::draw(
void VulkanMesh_lines::draw(VulkanCommandBuffer& commandBuffer)
{
	// bind pipeline
	VulkanMesh::draw(commandBuffer);

	// render
	vkCmdBindVertexBuffers(commandBuffer.commandBuffer, 0, 2, bufferHandles.data(), bufferOffsets.data());
	vkCmdDraw(commandBuffer.commandBuffer, vertexCount, 1, 0, 0);
};
