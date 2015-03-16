#version 150

out vec4 out_Color;

in vec3 out_Normal;

void main(void)
{
    out_Color =  vec4(normalize(out_Normal),1);
}
