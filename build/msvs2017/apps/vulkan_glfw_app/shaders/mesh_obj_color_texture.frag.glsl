#version 450
#extension GL_ARB_separate_shader_objects : enable

// inputs
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vTexCoords;
layout(location = 2) in vec3 vNormal;

// diffuse texture
layout(set = 0, binding = 0) uniform sampler2D diffuseTexture;

// material colors
layout(set = 0, binding = 1) uniform materialColors{
	vec4 diffuseColor;
	vec4 ambientColor;
	vec4 emissionColor;
	vec4 specularColor;
	float specularFactor;
} uMaterialColors;

// outputs
layout(location = 0) out vec4 fragColor;

// main
void main()
{
	fragColor = texture(diffuseTexture, vTexCoords);
	//fragColor = vec4(vPosition, 1.0f);
	//fragColor = vec4(vTexCoords, 0.0f, 1.0f);
	//fragColor = vec4(vNormal, 1.0f);
}
