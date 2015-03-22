#version 150

out vec4 out_Color;
in  vec3 ex_Color;

in vec2 texCoord;

uniform sampler2D texUnit0;
uniform sampler2D texUnit1;
uniform sampler2D texUnit2;

in vec3 normal;
in float intensity;
in vec3 out_Normal;
in vec3 pos;

in float distance;

uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];

const vec4 fogColor = vec4(0,0,0,0);

uniform float worldTime;

void main(void)
{
    vec4 ambient =vec4(0.1, 0.1, 0.1, 0.1);
    
    vec2 grid;
    if (normal.x==0) {
        grid = vec2(pos.x, pos.y + pos.z);
    } else {
        grid = vec2(pos.z, pos.y);
    }

    vec3 normal = 0.5 * normalize(out_Normal) + 2*vec3(texture(texUnit2, grid));
    normal = normalize(normal);

    vec4 material = texture(texUnit1, texCoord);
    float materialDiffuse = material.x;
    float materialSpecular = material.y;
    float materialSelfIllumination = material.z; 

    vec3 diffuse = vec3(0,0,0);

    vec3 eyeDir = normalize(-pos);
    vec3 specular = vec3(0,0,0);

    for (int i=0;i<4;i++) {
        // Diffuse
        vec3 lightDir;
        if (isDirectional[i])
            lightDir = normalize(lightSourcesDirPosArr[i]);
        else
            lightDir = normalize(lightSourcesDirPosArr[i]-pos);
        float diffuseStr = dot(normal,lightDir);
        diffuseStr = max(diffuseStr,0);
        diffuse += diffuseStr*lightSourcesColorArr[i];

        // Specular
        vec3 reflectedDir = reflect(lightDir,normal);
        float specularStr = dot(reflectedDir, eyeDir);
        float exp = specularExponent[i];
        if (specularStr > 0.0) {
            specularStr = pow(specularStr, exp);
        }
        specularStr = max(specularStr,0);
        specular += specularStr*lightSourcesColorArr[i];
    }
    diffuse *= materialDiffuse;
    specular *= materialSpecular;

    vec4 lighting = vec4( diffuse + specular,1)  + vec4(1,1,1,1) * materialSelfIllumination;

    float sDistance=distance;

    out_Color =  (texture(texUnit0, texCoord) * lighting);
}
