#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 dMVP;

void main()
{
    gl_Position =  dMVP * vec4(vertexPosition_modelspace,1);
}