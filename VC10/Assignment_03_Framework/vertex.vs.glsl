#version 410

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec3 iv2tex_coord;
layout(location = 2) in vec3 iv3normal;

uniform mat4 um4mvp;


out vec2 vv2tex_coord;
out vec3 vv3normal;

void main()
{
	gl_Position = um4mvp * vec4(iv3vertex, 1.0);
	vv2tex_coord = iv2tex_coord.xy;
	vv3normal = iv3normal ;
}