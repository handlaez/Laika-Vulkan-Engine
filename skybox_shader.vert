#version 450

layout(location = 0) in vec3 inPos;

layout(set = 0, binding = 0) uniform CameraUBO
{
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec3 texCoords;

void main()
{
    mat4 view = mat4(mat3(ubo.view));
    vec4 pos = ubo.proj * view * vec4(inPos, 1.0);
    gl_Position = pos.xyww;
    texCoords = vec3(inPos.x, -inPos.y, inPos.z);
}