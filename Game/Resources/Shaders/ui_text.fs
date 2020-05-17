#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 font_color;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(image, TexCoords).r);
	 if(sampled.a < 0.9)
        discard;
    color = vec4(font_color, 1.0) * sampled;
}
