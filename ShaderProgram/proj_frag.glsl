#version 410 core

layout(location=0) in vec4 projTexCoord;
layout(location=0) out vec3 color;
uniform sampler2D projTexture;

void main() 
{ 
  vec3 tcolor = vec3(0.3,0.3,0);
  if(projTexCoord.q > 0)
  {
    vec4 projectorColor = texture2DProj(projTexture,projTexCoord);
    //tcolor += projectorColor.xyz;
    tcolor += vec3(0,0,1.0);
    color = tcolor;
  }
}