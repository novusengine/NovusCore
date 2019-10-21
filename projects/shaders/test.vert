#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(binding = 0) uniform SharedUniformBufferObject 
{
    mat4 view;
    mat4 proj;
} sharedUbo;

layout(binding = 1) uniform ModelUniformBufferObject 
{
    mat4 model;
} modelUbo;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragColor;

void main() 
{
    gl_Position = sharedUbo.proj * sharedUbo.view * modelUbo.model * vec4(inPosition, 1.0);
	fragColor = vec3(1,0,0);
}