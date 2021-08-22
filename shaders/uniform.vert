#version 460

layout(binding = 0) uniform UniformBufferObject
{
    vec2 offset;
};

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

layout(location = 0) out vec4 vColor;

void main()
{
    vColor = vec4(aColor, 1.0);
    gl_Position = vec4(aPos + offset, 0.0, 1.0);
}