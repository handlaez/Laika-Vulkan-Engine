#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform Push {
    vec4 objectPos;
    vec4 objectForward;
    vec4 color;
} push;

layout(location = 0) out vec3 fragNorm;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragPos;

void main() {
    vec3 forward = normalize(push.objectForward.xyz);

    vec3 worldUp = abs(forward.y) > 0.99
        ? vec3(1.0, 0.0, 0.0)
        : vec3(0.0, 1.0, 0.0);

    vec3 right = normalize(cross(worldUp, forward));
    vec3 up = cross(forward, right);

    mat3 rotation = mat3(right, up, forward);

    vec3 worldPos = rotation * inPos + push.objectPos.xyz;
    fragPos = worldPos;
    
    gl_Position = ubo.proj * ubo.view * vec4(worldPos, 1.0);

    fragNorm = rotation * inNormal;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}