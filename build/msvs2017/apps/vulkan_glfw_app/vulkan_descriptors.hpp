#pragma once

#include <vktoolkit.hpp>

//////////////////////////////////////////////////////////////////////////

// vertex structure
typedef struct VertexStruct_P4_C4_T2 {
	float X, Y, Z, W; 
	float R, G, B, A; 
	float U, V; 
} VertexStruct_P4_C4_T2;

// VkVertexInputBindingDescription
const VkVertexInputBindingDescription vertexBindingDescriptions_P4_C4_T2[] {
{ 0, sizeof(VertexStruct_P4_C4_T2), VK_VERTEX_INPUT_RATE_VERTEX },
};

// VkVertexInputAttributeDescription
const VkVertexInputAttributeDescription vertexAttributeDescriptions_P4_C4_T2[]{
{ 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT,  0 }, // position - 4
{ 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 16 }, // color    - 4
{ 2, 0, VK_FORMAT_R32G32_SFLOAT      , 32 }, // texCoord - 2
};

//////////////////////////////////////////////////////////////////////////

// VkVertexInputBindingDescription
const VkVertexInputBindingDescription vertexBindingDescriptions_mesh_obj[]{
{ 0, sizeof(float) * 4, VK_VERTEX_INPUT_RATE_VERTEX },
{ 1, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_VERTEX },
{ 2, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX },
{ 3, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX },
{ 4, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX },
};

// VkVertexInputAttributeDescription
const VkVertexInputAttributeDescription vertexAttributeDescriptions_mesh_obj[]{
{ 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0 }, // position - 4
{ 1, 1, VK_FORMAT_R32G32_SFLOAT      , 0 }, // texCoord - 2
{ 2, 2, VK_FORMAT_R32G32B32_SFLOAT   , 0 }, // normal   - 3
{ 3, 3, VK_FORMAT_R32G32B32_SFLOAT   , 0 }, // tangent  - 3
{ 4, 4, VK_FORMAT_R32G32B32_SFLOAT   , 0 }, // binormal - 3
};

//////////////////////////////////////////////////////////////////////////

// VkVertexInputBindingDescription
const VkVertexInputBindingDescription vertexBindingDescriptions_mesh_obj_skin[]{
{ 0, sizeof(float) * 4, VK_VERTEX_INPUT_RATE_VERTEX },
{ 1, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_VERTEX },
{ 2, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX },
{ 3, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX },
{ 4, sizeof(float) * 3, VK_VERTEX_INPUT_RATE_VERTEX },
{ 5, sizeof(float) * 4, VK_VERTEX_INPUT_RATE_VERTEX },
{ 6, sizeof(float) * 4, VK_VERTEX_INPUT_RATE_VERTEX },
};

// VkVertexInputAttributeDescription
const VkVertexInputAttributeDescription vertexAttributeDescriptions_mesh_obj_skin[]{
{ 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0 }, // position - 4
{ 1, 1, VK_FORMAT_R32G32_SFLOAT      , 0 }, // texCoord - 2
{ 2, 2, VK_FORMAT_R32G32B32_SFLOAT   , 0 }, // normal   - 3
{ 3, 3, VK_FORMAT_R32G32B32_SFLOAT   , 0 }, // tangent  - 3
{ 4, 4, VK_FORMAT_R32G32B32_SFLOAT   , 0 }, // binormal - 3
{ 5, 5, VK_FORMAT_R32G32B32A32_SFLOAT, 0 }, // weights  - 4
{ 6, 6, VK_FORMAT_R32G32B32A32_SFLOAT, 0 }, // indexes  - 4
};

//////////////////////////////////////////////////////////////////////////

// VkVertexInputBindingDescription
const VkVertexInputBindingDescription vertexBindingDescriptions_P4_C4[]{
{ 0, sizeof(float) * 4, VK_VERTEX_INPUT_RATE_VERTEX },
{ 1, sizeof(float) * 4, VK_VERTEX_INPUT_RATE_VERTEX },
};

// VkVertexInputAttributeDescription
const VkVertexInputAttributeDescription vertexInputAttributeDescriptions_P4_C4[]{
{ 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0 }, // position - 4
{ 1, 1, VK_FORMAT_R32G32B32A32_SFLOAT, 0 }, // color    - 4
};

//////////////////////////////////////////////////////////////////////////

// VkDescriptorSetLayoutBinding - Material set
const VkDescriptorSetLayoutBinding descriptorSetLayoutBindings_material[]{
{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, VK_NULL_HANDLE }, // diffuse texture
{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1, VK_SHADER_STAGE_FRAGMENT_BIT, VK_NULL_HANDLE }, // material colors
};

// VkDescriptorSetLayoutBinding - Model set
const VkDescriptorSetLayoutBinding descriptorSetLayoutBindings_model[]{
{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE }, // model matrix
};

// VkDescriptorSetLayoutBinding - Scene set
const VkDescriptorSetLayoutBinding descriptorSetLayoutBindings_scene[]{
{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE }, // camera (view, projection)
{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, VK_NULL_HANDLE }, // light sources
};

//////////////////////////////////////////////////////////////////////////

// VkPipelineColorBlendAttachmentState
const VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentStates_default[]{
	{ // first attachments
		VK_FALSE,
		VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	}
};

// vulkanDescriptorsInit
void vulkanDescriptorsInit();