#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

class GLMesh
{
public:
  GLMesh(Vector<vec3> vertexList);
  void render();
  void setColor(vec3 color);
private:  
  Vector<vec3> vertices;
};