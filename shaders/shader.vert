#version 460

layout (binding = 0) uniform SceneBuffer
{
	mat4 cameraMatrix;
} sceneBuffer;

layout (binding = 1) uniform ModelBuffer
{
	mat4 bones[256];
};

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexcoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec4 inBoneWeights;
layout (location = 4) in uvec4 inBoneIndices;

layout (location = 0) out vec3 fragPosition;
layout (location = 1) out vec3 fragNormal;
layout (location = 2) out vec2 fragTexcoord;

void main()
{
	vec4 pos = vec4(inPosition, 1.0);
	pos =
				bones[inBoneIndices.x] * pos * inBoneWeights.x + 
				bones[inBoneIndices.y] * pos * inBoneWeights.y + 
				bones[inBoneIndices.z] * pos * inBoneWeights.z + 
				bones[inBoneIndices.w] * pos * inBoneWeights.w;
	fragPosition = pos.xyz;
	gl_Position = sceneBuffer.cameraMatrix * pos;
	fragTexcoord = inTexcoord;
	fragNormal = inNormal;
}
