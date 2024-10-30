#version 460

layout (binding = 0) uniform SceneBuffer
{
	mat4 cameraMatrix;
} sceneBuffer;

layout (binding = 1) uniform ModelBuffer
{
	mat4 bones[100];
};

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexcoord;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec3 fragPosition;
layout (location = 1) out vec3 fragNormal;
layout (location = 2) out vec2 fragTexcoord;

void main()
{
	vec4 pos = vec4(inPosition, 1.0);
	fragPosition = pos.xyz;
	gl_Position = sceneBuffer.cameraMatrix * pos;
	fragTexcoord = inTexcoord;
	fragNormal = inNormal;
}
