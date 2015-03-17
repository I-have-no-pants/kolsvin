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

const float worldTime = 1000;

void main(void)
{
    // Notice that this breaks lighting
    vec4 position = myMatrix*vec4(in_Position, 1.0);

    // Trippy shit going on here
    float distanceSquared = position.x * position.x + position.y*position.y;
    //position.y += 5*sin(distanceSquared*sin(float(worldTime)/143.0)/1000);
    
    float y = position.y;
    float x = position.x;
    float om = sin(distanceSquared*sin(float(worldTime)/256.0)/5000) * sin(float(worldTime)/200.0);
    //position.y = x*sin(om)+y*cos(om);
    //position.x = x*cos(om)-y*sin(om);

    gl_Position = projectionMatrix * CameraMatrix * position;
    pos = vec3(position);
    
    vec3 normal = mat3(myMatrix) * in_Normal;

    vec3 lightDirection = normalize(-in_Position + light);

    intensity = clamp(dot(normal, lightDirection),0 ,1);

    //ex_Color = 0.5*in_Normal +0.5*vec3(gl_Position.x,gl_Position.y, gl_Position.z);
    ex_Color = in_Normal *( -gl_Position.z + 0.25);
   texCoord = in_TexCoord;

    out_Normal = normal;
}
