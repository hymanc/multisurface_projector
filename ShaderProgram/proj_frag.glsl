#version 410 core

layout(location=0) in vec4 projTexCoord;
layout(location=1) in vec2 UV;

layout(location=0) out vec3 color;

uniform sampler2D projTexture;

void main() 
{ 
  vec3 tcolor = vec3(0.1,0.1,0.1);
  if(projTexCoord.q > 0)
  {
    // Use += for 'blend' with ambient = for projection only
    tcolor += textureProj(projTexture,projTexCoord).rgb;
  }
  color = tcolor;
}