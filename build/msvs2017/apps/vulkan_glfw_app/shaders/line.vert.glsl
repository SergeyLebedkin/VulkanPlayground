#version 450
#extension GL_ARB_separate_shader_objects : enable

// attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

// outputs
layout(location = 0) out vec3 vPosition;
layout(location = 1) out vec4 vColor;

// uniforms
layout(binding = 0) uniform buffer1{
	mat4 model;
	mat4 view;
	mat4 proj;
} uMatrices;

// main
void main()
{
	// copy in to out
	vPosition = aPosition;
	vColor = aColor;

	// find position
	//gl_Position = aPosition;
	gl_Position = uMatrices.proj * uMatrices.view * uMatrices.model * vec4(aPosition, 1.0f);
	//gl_Position = vec4(aPosition.xyz/16.0f, 1.0f);
}
