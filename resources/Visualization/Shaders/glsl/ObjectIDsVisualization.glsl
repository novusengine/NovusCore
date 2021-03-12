// This Shader is used to visualize ObjectIDs in RenderDoc (Confirmed working as of v1.11)

#version 420 core

#define RED_SEED 3
#define GREEN_SEED 5
#define BLUE_SEED 7

layout(binding = 0, std140) uniform RENDERDOC_Uniforms
{
        uvec4 TexDim;
        uint SelectedMip;
        uint TextureType;
        uint SelectedSliceFace;
        int SelectedSample;
        uvec4 YUVDownsampleRate;
        uvec4 YUVAChannels;
} RENDERDOC;

layout(binding = 12) uniform usampler2DArray texUInt2DArray;

layout (location = 0) in vec2 uv;
layout (location = 0) out vec4 color_out;

highp float IDToColor(uint ID, uint seed)
{
    return float(ID % seed) / float(seed);
}

void main()
{
    uint value = texture(texUInt2DArray, vec3(uv, 0)).r;

    uint typeID = value >> 28;
    uint objectID = value & 0x0FFFFFFF;

    vec3 color = vec3(0,0,0);
    color.r = IDToColor(objectID, RED_SEED);
    color.g = IDToColor(objectID, GREEN_SEED);
    color.b = IDToColor(objectID, BLUE_SEED);

    color_out = vec4(color,typeID+1);
}