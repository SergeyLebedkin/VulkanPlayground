#version 450
#extension GL_ARB_separate_shader_objects : enable

// inputs
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vColor;

// outputs
layout(location = 0) out vec4 fragColor;

// main
void main()
{
	fragColor = vColor;
	//fragColor = vec4(vPosition, 1.0f);
	//fragColor = vec4(vTexCoords, 0.0f, 1.0f);
	//fragColor = vec4(vNormal, 1.0f);
}
