#version 330 core

layout(location = 0) out vec3 color;

in vec2 UV;
in vec3 Position_worldspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec4 shadowCoord;

uniform sampler2D textureSampler;
mat4 MV;
uniform vec3 LightPosition_worldspace;
uniform sampler2DShadow projMap;

void main()
{
  if ( textureProj( shadowMap, ShadowCoord.xyw ).z  <  (ShadowCoord.z-bias)/ShadowCoord.w )
    color = vec3(0.4,0.2,0);
}