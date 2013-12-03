#version 410 core

layout(location=0) in vec4 projTexCoord;
layout(location=1) in vec4 vertPos;
layout(location=0) out vec3 color;
uniform sampler2D projTexture;

void main() 
{ 
  vec3 tcolor = vec3(0.3,0.3,0);
  if(projTexCoord.q > 0)
  {
    vec4 projectorColor = textureProj(projTexture,projTexCoord);
    tcolor += projectorColor.rgb;
    //tcolor += vec3(0,0,1.0);
    //tcolor += projTexCoord.rgb;
  }
  color = tcolor;//texture2Dproj(projTexture,projTexCoord).rgb;
}