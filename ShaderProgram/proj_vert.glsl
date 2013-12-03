#version 410 core
//#extension GL_ARB_separate_shader_objects

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 0) out vec4 projTexCoord;

uniform mat4 MVP;
uniform mat4 texMVP;

void main()
{
  projTexCoord = texMVP * vec4(vertexPosition_modelspace, 1);
  projTexCoord.xy = 0.5*projTexCoord.xy + 0.5*projTexCoord.w; // Perform bias
  gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
  
  //gl_FrontColor = gl_Color;
  //gl_Position =  dMVP * vec4(vertexPosition_modelspace,1);
  // Compute perspective projector frame texture space
  //vec4 worldPos = InvertedView * gl_ModelViewMatrix * gl_Vertex;
  //gl_TexCoord[0] = TextureProjMat * worldPos;
  // Do view camera transform
  
}