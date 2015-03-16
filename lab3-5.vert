#version 150

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoord;

uniform mat4 myMatrix;
uniform mat4 projectionMatrix;
uniform mat4 CameraMatrix;

out vec3 out_Normal;
out vec2 texCoord;
out vec3 ex_Color;
out vec3 pos;

out float intensity;

const vec3 light = vec3(30.58, 30.58, -30.58);

void main(void)
{
    vec4 position = myMatrix*vec4(in_Position, 1.0);
	gl_Position = projectionMatrix * CameraMatrix *position;
    pos = vec3(position);
    
    vec3 normal = mat3(myMatrix) * in_Normal;

    vec3 lightDirection = normalize(-in_Position + light);

    intensity = clamp(dot(normal, lightDirection),0 ,1);

    //ex_Color = 0.5*in_Normal +0.5*vec3(gl_Position.x,gl_Position.y, gl_Position.z);
    ex_Color = in_Normal *( -gl_Position.z + 0.25);
   texCoord = in_TexCoord;

    out_Normal = normal;
}
