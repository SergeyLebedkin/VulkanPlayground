#version 450
#extension GL_ARB_separate_shader_objects : enable

// inputs
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoords;
layout(location = 2) in vec3 vNormal;

// uniforms
layout(binding = 0) uniform sampler2D texSampler;

// outputs
layout(location = 0) out vec4 fragColor;

// main
void main()
{	
	fragColor = texture(texSampler, vTexCoords);
	//fragColor = vec4(vTexCoords, 0.0f, 1.0f);
}
