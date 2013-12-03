#version 330

uniform mat4 dMVP;
uniform mat4 InvViewMat;
varying vec3 normal, lightDir, eyeVec;

void main()
{
  normal = gl_Normal_matrix * gl_Normal;
  
  vec4 posEye = gl_ModelViewMatrix * gl_Vertex;
  vec4 posWorld = InvViewMat * posEye;
  gl_TexCoord[0] = dMVP

}