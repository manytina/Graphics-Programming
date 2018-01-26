#version 420

layout (binding = 0) uniform sampler2D origin;
layout (binding = 1) uniform sampler2D tex;
uniform vec2 img_size;
uniform int shader_now;
out vec4 fragColor;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

void main()
{
//if(shader_now == 0){
		fragColor = vec4(0);
		int n = 0;
		int half_size = 8;

		for(int i=-half_size; i<=half_size; i++){
			for(int j=-half_size; j<= half_size; j++){
				vec4 c = texture(origin, fs_in.texcoord+vec2(i,j)/img_size);
				fragColor += c;
				n++;
			}
		}

		fragColor /= n;
//	}
	if(shader_now == 1){
		vec3 hdrColor = texture(origin, vec2(fs_in.texcoord.x+0.005, fs_in.texcoord.y+0.005)).rgb * 0.2;      
		vec3 bloomColor = texture(tex, fs_in.texcoord).rgb * 0.8;
		hdrColor += bloomColor; // additive blending
		int nbins = 8;
		vec4 tex_color = vec4(hdrColor, 1.0);
		float r = floor(tex_color.r * float(nbins)) / float(nbins);
		float g = floor(tex_color.g * float(nbins)) / float(nbins);
		float b = floor(tex_color.b * float(nbins)) / float(nbins);

		fragColor = vec4(r, g, b, tex_color.a);
	
	//	fragColor = vec4(bloomColor, 1.0);
	}
}