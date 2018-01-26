#version 420

layout (binding = 0) uniform sampler2D tex;
layout (binding = 1) uniform sampler2D fbo;

uniform float linePos;
layout (location = 0) out vec4 fragColor;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

void main()
{
//	float linePos = 0.5;

	if(fs_in.texcoord.x - linePos <= 0.005 && fs_in.texcoord.x - linePos >= -0.005){
		fragColor = vec4(1.0, 1.0, 0.0, 1.0);
	}
	else if(fs_in.texcoord.x > linePos){
		fragColor = texture(tex, fs_in.texcoord);
	}
	else{
		fragColor = texture(fbo, fs_in.texcoord);
	}
}