#version 410

uniform sampler2D tex;
uniform float iGlobalTime;
out vec4 fragColor;

float horizon = 0.25;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

void main()
{
	float sepoffset = 0.005 * cos(iGlobalTime * 3.0);
	fragColor = texture(tex, vec2(fs_in.texcoord.x, fs_in.texcoord.y));
	
	if(fs_in.texcoord.y <= horizon + sepoffset){
        float xoffset = 0.005 * cos(iGlobalTime * 3.0 + 200.0 * fs_in.texcoord.y);
		vec2 mirror = vec2(fs_in.texcoord.x, 2 * horizon - fs_in.texcoord.y);
        float yoffset =  0.005 * (1.0 + cos(iGlobalTime * 3.0 + 50.0 * fs_in.texcoord.y));
    	vec4 color = texture(tex, vec2(mirror.x+xoffset , mirror.y+yoffset));
    	fragColor = color;
	}
}
