#version 460

layout (set = 0, binding = 0) uniform sampler2D texture0;

layout (location = 0) in vec2 vTexCoord;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = texture(texture0, vTexCoord);
}