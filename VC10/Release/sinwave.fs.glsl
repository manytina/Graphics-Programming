#version 410

uniform sampler2D tex;
uniform float offset;
out vec4 fragColor;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

void main()
{
/*	int n = 0;
	float half_size = 0.003f;
	fragColor = vec4(0.0);
	float filter_avg = 0.0;

	vec4 texcolor = texture(tex, fs_in.texcoord);
	float filter_threshold = 0.2126 * texcolor.r + 0.7152 * texcolor.g + 0.0722 * texcolor.b;
	for(float i=-half_size; i<=half_size; i+=0.001){
		for(float j=-half_size; j<= half_size; j+=0.001){			
			vec4 c = texture(tex, fs_in.texcoord+vec2(i,j));
			float grayscale_color = 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b;
			filter_avg += grayscale_color;
		//	fragColor += c;
			n++;
		}
	}

	filter_avg /= n;
	fragColor = (filter_threshold > filter_avg) ? vec4(0.0, 0.0, 0.0, 1.0) : vec4(1.0, 1.0, 1.0, 1.0);
*/

	fragColor = texture(tex, fs_in.texcoord+vec2(0.1 * sin(fs_in.texcoord.y * 3.14159 + offset), 0));
}