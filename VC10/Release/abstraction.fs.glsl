#version 410

uniform sampler2D tex;
uniform vec2 img_size;
uniform int shader_now;
//layout (location = 0) out vec4 color0;
//layout (location = 1) out vec4 color1;
//layout (location = 2) out vec4 color2;
vec4 color0, color1, color2;
out vec4 fragColor;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

int BLUR = 0;
int QUANTIZATION = 1;

float sigma_e = 2.0f;
float sigma_r = 2.8f;
float phi = 3.4f;
float tau = 0.99f;
float twoSigmaESquared = 2.0 * sigma_e * sigma_e;	
float twoSigmaRSquared = 2.0 * sigma_r * sigma_r;		
int halfWidth = int(ceil( 2.0 * sigma_r ));

void main()
{
		color0 = vec4(0);
		int n = 0;
		int half_size = 4;

		for(int i=-half_size; i<=half_size; i++){
			for(int j=-half_size; j<= half_size; j++){
				vec4 c = texture(tex, fs_in.texcoord+vec2(i,j)/img_size);
				color0 += c;
				n++;
			}
		}

		color0 /= n;

		int nbins = 8;

	//	vec4 tex_color = texture(tex, fs_in.texcoord);
		vec4 tex_color = color0;
		float r = floor(tex_color.r * float(nbins)) / float(nbins);
		float g = floor(tex_color.g * float(nbins)) / float(nbins);
		float b = floor(tex_color.b * float(nbins)) / float(nbins);

		color1 = vec4(r, g, b, tex_color.a);
	//	fragColor = color1;
	

		vec2 sum = vec2(0.0);
		vec2 norm = vec2(0.0);

		for(int i=-halfWidth; i<=halfWidth; i++){
			for(int j=-halfWidth; j<=halfWidth; j++){
				float d = length(vec2(i, j));
				vec2 kernel = vec2(exp(-d*d/twoSigmaESquared), exp(-d*d/twoSigmaRSquared));
				vec4 c = texture(tex, fs_in.texcoord+vec2(i,j)/img_size);
				vec2 L = vec2(0.299*c.r+0.587*c.g+0.114*c.b);
				norm += 2.0 * kernel;
				sum += kernel * L;
			}
		}

		sum /= norm;
		float H = 100.0 * (sum.x - tau * sum.y);
		float e = 2.0 * smoothstep(-2.0, 2.0, phi*H);		
		vec4 temp = vec4(1.0, 1.0, 1.0, 1.0);
		color2 = (H > 0.0 || e != 0.0) ? color1 : vec4(e, e, e, 1);
		
		fragColor = color2;
	

}