#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    vec4 color;
} push;

layout(location = 0) out vec3 fragNorm;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragPos;

void main() {
    vec4 worldPos = push.modelMatrix * vec4(inPos, 1.0);
    fragPos = worldPos.xyz;
    
    fragNorm = mat3(push.modelMatrix) * inNormal;

    gl_Position = ubo.proj * ubo.view * worldPos;

    fragColor = inColor;
    fragTexCoord = inTexCoord;
}