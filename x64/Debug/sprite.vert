#version 460 core

layout(std140, binding = 1) uniform transform
{
	mat4 MVP;
} Transform;

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;
//layout(location = 2) in vec4 color;

out block
{
	vec2 texcoord;
	//vec4 color;
} Out;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{	
	gl_Position = Transform.MVP * vec4(position, 0.0, 1.0);
	Out.texcoord = texcoord;
	//Out.color = color;
}