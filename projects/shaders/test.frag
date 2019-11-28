#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() 
{
    //outColor = vec4(fragTexCoord, 0.0, 1.0); // Debug Texcoords
	outColor = texture(texSampler, fragTexCoord);
}