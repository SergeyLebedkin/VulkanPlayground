#version 450
#extension GL_ARB_separate_shader_objects : enable

// attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;

// outputs
layout(location = 0) out vec3 vPosition;
layout(location = 1) out vec2 vTexCoords;
layout(location = 2) out vec3 vNormal;

// uniforms
layout(binding = 1) uniform buffer1 {
	mat4 model;
	mat4 view;
	mat4 proj;
} uMatrices;

// main
void main()
{
	// copy in to out
	vPosition = aPosition;
	vTexCoords = aTexCoords;
	vNormal = aNormal;

	// find position
	//gl_Position = aPosition;
	vec4 pos = vec4(aPosition, 1.0f);
	gl_Position = uMatrices.proj * uMatrices.view * uMatrices.model * vec4(aPosition, 1.0f);
	//gl_Position = vec4(aPosition.xyz/16.0f, 1.0f);
}
