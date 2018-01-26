#version 410

uniform sampler2D tex;
vec4 Right_Color, Left_Color;
layout (location = 0) out vec4 fragColor;

float offset = 0.005;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

void main()
{
	Left_Color = texture(tex,fs_in.texcoord-offset);
	Right_Color = texture(tex,fs_in.texcoord+offset);

	float Color_R= Left_Color.r*0.299+Left_Color.g*0.587+Left_Color.b*0.114;
	float Color_G= Right_Color.g;
	float Color_B= Right_Color.b;

	fragColor = vec4(Color_R,Color_G,Color_B,1.0);
}