#version 410

uniform sampler2D tex;
layout (location = 0) out vec4 fragColor;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

void main()
{
	int nbins = 8;

	vec4 tex_color = texture(tex, fs_in.texcoord);
	float r = floor(tex_color.r * float(nbins)) / float(nbins);
	float g = floor(tex_color.g * float(nbins)) / float(nbins);
	float b = floor(tex_color.b * float(nbins)) / float(nbins);

	fragColor = vec4(r, g, b, tex_color.a);
}