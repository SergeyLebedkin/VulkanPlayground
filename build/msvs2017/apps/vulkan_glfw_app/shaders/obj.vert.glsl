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
layout(binding = 1) uniform buffer0 {
	mat4 uWVP;
} matrices;

// main
void main()
{
	// copy in to out
	vPosition = aPosition;
	vTexCoords = aTexCoords;
	vNormal = aNormal;

	// find position
	//gl_Position = aPosition;
	//gl_Position = matrices.uWVP * aPosition;
	gl_Position = vec4(aPosition.xyz/16.0f, 1.0f);
}
