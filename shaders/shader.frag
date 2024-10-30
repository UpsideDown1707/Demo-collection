#version 460

layout (binding = 2) uniform SceneBuffer
{
	vec4 lightPosition;
	vec4 lightColor;
} sceneBuffer;

layout (binding = 3) uniform ModelBuffer
{
	vec4 diffuseColor;
	vec3 specularColor;
	float specularPower;
} modelBuffer;

layout (binding = 4) uniform sampler2D texSampler;

layout (location = 0) in vec3 fragPosition;
layout (location = 1) in vec3 fragNormal;
layout (location = 2) in vec2 fragTexcoord;

layout (location = 0) out vec4 outColor;

void main()
{
	float shade = clamp(0.0f, 1.0f, dot(fragNormal, normalize(sceneBuffer.lightPosition.xyz - fragPosition)));
	vec4 color = texture(texSampler, fragTexcoord);
	outColor = (shade * 0.5f + 0.5f) * color;
}
