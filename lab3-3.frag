#version 150

out vec4 out_Color;
in  vec3 ex_Color;

in vec2 texCoord;

uniform sampler2D texUnit;

in vec3 normal;
in float intensity;

in vec3 out_Normal;

void main(void)
{
    out_Color =  texture(texUnit, texCoord); // * (intensity + 0.1); // vec4(out_Normal,1.0); //
}
