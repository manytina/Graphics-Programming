#version 410

uniform sampler2D tex;
out vec4 fragColor;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

void main()
{
	int n = 0;
	float half_size = 0.02f;
	fragColor = vec4(0.0);

	int beginX = int(floor(fs_in.texcoord.x / half_size));
	int beginY = int(floor(fs_in.texcoord.y / half_size));

	for(float i=beginX*half_size; i<(beginX+1)*half_size; i+=0.001f){
		for(float j=beginY*half_size; j<(beginY+1)*half_size; j+=0.001f){
			vec4 c = texture(tex, vec2(i,j));
			fragColor += c;
			n++;	
		}
	}

	fragColor /= n;
//	fragColor = texture(tex,vec2(fs_in.texcoord.x+0.5,fs_in.texcoord.y+0.5));

/*
//	vec4 texcolor = texture(tex, fs_in.texcoord);
//	float filter_threshold = 0.2126 * texcolor.r + 0.7152 * texcolor.g + 0.0722 * texcolor.b;
	for(int i=-half_size; i<=half_size; i++){
		for(int j=-half_size; j<= half_size; j++){			
			vec4 c = texture(tex, fs_in.texcoord+vec2(i,j)/img_size);
		//	float grayscale_color = 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b;
		//	filter_avg += grayscale_color;
			fragColor += c;
			n++;
		}
	}

	filter_avg /= n;
	fragColor = (filter_threshold > filter_avg) ? vec4(0.0, 0.0, 0.0, 1.0) : vec4(1.0, 1.0, 1.0, 1.0);
*/
}