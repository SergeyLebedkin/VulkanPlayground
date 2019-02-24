#version 450
#extension GL_ARB_separate_shader_objects : enable

// attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

// outputs
layout(location = 0) out vec3 vPosition;
layout(location = 1) out vec4 vColor;

// model uniforms
layout(set = 1, binding = 0) uniform buffer0{
	mat4 model;
} uModelMatrices;

// scene uniforms
layout(set = 2, binding = 0) uniform buffer1{
	mat4 view;
	mat4 proj;
} uSceneMatrices;

// main
void main()
{
	// copy in to out
	vPosition = aPosition;
	vColor = aColor;

	// find position
	//gl_Position = aPosition;
	gl_Position =
		uSceneMatrices.proj *
		uSceneMatrices.view *
		uModelMatrices.model * vec4(aPosition, 1.0f);
}
