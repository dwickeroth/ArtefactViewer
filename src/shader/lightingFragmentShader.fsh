#version 150

uniform vec4 ambientColor;
uniform vec4 diffuse0Color;
uniform vec4 diffuse1Color;
uniform vec4 diffuse2Color;
uniform vec4 diffuse3Color;
uniform vec4 specular0Color;
uniform vec4 specular1Color;
uniform vec4 specular2Color;
uniform vec4 specular3Color;
uniform float ambientReflection;
uniform float diffuseReflection;
uniform float specularReflection;

uniform float shininess;
//uniform sampler2D texture;

in vec3 varyingNormal;
in vec3 varyingLight0Direction;
in vec3 varyingLight1Direction;
in vec3 varyingLight2Direction;
in vec3 varyingLight3Direction;
in vec3 varyingViewerDirection;
//in vec2 varyingTextureCoordinate;
in vec4 varyingColor;

out vec4 fragColor;

void main(void)
{
        vec3 normal = normalize(varyingNormal);
        vec3 light0Direction = normalize(varyingLight0Direction);
        vec3 light1Direction = normalize(varyingLight1Direction);
        vec3 light2Direction = normalize(varyingLight2Direction);
        vec3 light3Direction = normalize(varyingLight3Direction);
        vec3 viewerDirection = normalize(varyingViewerDirection);

        vec4 ambientIllumination = ambientReflection * ambientColor;

        vec4 diffuseIllumination = ((diffuseReflection * max(0.0, dot(light0Direction, normal)) * diffuse0Color) +
                                    (diffuseReflection * max(0.0, dot(light1Direction, normal)) * diffuse1Color) +
                                    (diffuseReflection * max(0.0, dot(light2Direction, normal)) * diffuse2Color) +
                                    (diffuseReflection * max(0.0, dot(light3Direction, normal)) * diffuse3Color));

        vec4 specularIllumination = specular0Color * specularReflection * pow(max(dot(normalize(light0Direction + viewerDirection), normal), 0.0), shininess) +
                                    specular1Color * specularReflection * pow(max(dot(normalize(light1Direction + viewerDirection), normal), 0.0), shininess) +
                                    specular2Color * specularReflection * pow(max(dot(normalize(light2Direction + viewerDirection), normal), 0.0), shininess) +
                                    specular3Color * specularReflection * pow(max(dot(normalize(light3Direction + viewerDirection), normal), 0.0), shininess);

        fragColor = varyingColor * (ambientIllumination + diffuseIllumination + specularIllumination);

}
