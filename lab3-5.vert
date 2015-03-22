#version 150

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoord;

uniform mat4 myMatrix;
uniform mat4 myLODMatrix;
uniform mat4 projectionMatrix;
uniform mat4 CameraMatrix;

out vec3 out_Normal;
out vec2 texCoord;
out vec3 ex_Color;
out vec3 pos;

out float intensity;

out float distance;

const vec3 light = vec3(30.58, 30.58, -30.58);

uniform float worldTime;

void main(void)
{
    // Notice that this breaks lighting
    vec4 position = myMatrix*vec4(in_Position, 1.0);
    vec4 worldPosition = projectionMatrix * CameraMatrix * position;


    float distanceSquared = worldPosition.z*worldPosition.z + worldPosition.x * worldPosition.x + worldPosition.y*worldPosition.y;
    distance = distanceSquared*distanceSquared/40000000;
    float distanceC = clamp(distance , 0, 1);

    mat4 mergedMatrix = (1-distanceC) * myMatrix + distanceC * myLODMatrix;    

    gl_Position = projectionMatrix * CameraMatrix * mergedMatrix * vec4(in_Position, 1);


    pos = vec3(position);
    
    vec3 normal = mat3(mergedMatrix) * in_Normal;

    vec3 lightDirection = normalize(-in_Position + light);

    intensity = clamp(dot(normal, lightDirection),0 ,1);

    //ex_Color = 0.5*in_Normal +0.5*vec3(gl_Position.x,gl_Position.y, gl_Position.z);
    ex_Color = in_Normal *( -gl_Position.z + 0.25);
   texCoord = in_TexCoord;

    out_Normal = normal;
}
