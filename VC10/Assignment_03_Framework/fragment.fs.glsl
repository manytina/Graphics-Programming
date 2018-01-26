#version 410


layout(location = 0) out vec4 fragColor;


in vec2 vv2tex_coord;
in vec3 vv3normal;
uniform sampler2D tex;

void main()
{
    fragColor = texture(tex, vv2tex_coord);
}