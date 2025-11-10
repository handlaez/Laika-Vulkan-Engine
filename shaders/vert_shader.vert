#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(binding = 0) uniform UniformBufferObject {
	mat4 view;
	mat4 proj;
} ubo;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(push_constant) uniform Push {
	mat4 transform;
	vec3 color;
} push;

void main() {
	gl_Position = ubo.proj * ubo.view * push.transform * vec4(inPos, 1.0f);
	fragColor = inColor;
	fragTexCoord = inTexCoord;
}