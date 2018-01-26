#version 410

uniform sampler2D tex;
uniform vec2 mouse;
out vec4 fragColor;

float radius = 0.15;
float scale = 2.0;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

void main()
{
//	fragColor = texture(tex, fs_in.texcoord+vec2(0.1 * sin(fs_in.texcoord.y * 3.14159 + offset), 0));

//	vec2 center = vec2(0.3, 0.8);
	vec2 center = mouse;

	vec2 arrow = vec2(fs_in.texcoord.x - center.x, fs_in.texcoord.y - center.y);
	float dist = length(arrow);
	vec2 new_coord = (dist <= radius - 0.005) ? vec2(float(arrow.x/scale)+ center.x, float(arrow.y/scale)+center.y) : (radius - 0.005 < dist && dist <= radius) ? vec4(1.0, 1.0, 0.0, 1.0) : fs_in.texcoord;
	fragColor = texture(tex, new_coord);
}