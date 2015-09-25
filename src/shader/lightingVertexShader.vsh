#version 150

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform vec3 light0Position;
uniform vec3 light1Position;
uniform vec3 light2Position;
uniform vec3 light3Position;

in vec4 vertex;
in vec3 normal;
//in vec2 textureCoordinate;
in vec4 color;

out vec4 varyingColor;
out vec3 varyingNormal;
out vec3 varyingLight0Direction;
out vec3 varyingLight1Direction;
out vec3 varyingLight2Direction;
out vec3 varyingLight3Direction;
out vec3 varyingViewerDirection;
//out vec2 varyingTextureCoordinate;

void main(void)
{
    vec4 eyeVertex = mvMatrix * vertex;
    eyeVertex /= eyeVertex.w;
    varyingNormal = normalMatrix * normal;
    varyingLight0Direction = light0Position - eyeVertex.xyz;
    varyingLight1Direction = light1Position - eyeVertex.xyz;
    varyingLight2Direction = light2Position - eyeVertex.xyz;
    varyingLight3Direction = light3Position - eyeVertex.xyz;
    varyingViewerDirection = -eyeVertex.xyz;
//    varyingTextureCoordinate = textureCoordinate;
    varyingColor = color;
    gl_Position = mvpMatrix * vertex;
}
