#version 460 core

layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D sprite;

in block
{
	vec2 texcoord;
	//vec4 color;
} In;



void main()
{
	color = texture(sprite, In.texcoord);// * In.color;
}