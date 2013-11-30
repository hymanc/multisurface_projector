#include "ShaderProgram.h"
#include <GL/glfw.h>

using namespace std;
using namespace glm;
int main(int argc, char ** argv)
{
  const char * vshaderPath = "/home/cody/dev/multisurface_projector/ShaderProgram/vert.glsl";
  const char * fshaderPath = "/home/cody/dev/multisurface_projector/ShaderProgram/frag.glsl";
  
  if(!glfwInit())
  {
    fprintf(stderr, "Error initializing GLFW\n");
    return -1;
  }
  glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR,3);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR,3);
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  if(!glfwOpenWindow(1024, 768, 0,0,0,0,32,0,GLFW_WINDOW))
  {
    fprintf(stderr, "Error creating window\n");
    glfwTerminate();
    return -1;
  }
  glewExperimental = true;
  if(glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Error initializing GLEW\n");
    return -1;
  }
  glfwSetWindowTitle("Shader test");
  glfwEnable(GLFW_STICKY_KEYS);
  
  cout << "Staring test of shader load\n";
  GLuint progId = ShaderProgram::loadShader(vshaderPath, fshaderPath);
  cout << "Shader program compiled and loaded\n";
  
  GLuint vertexArrayId;
  glGenVertexArrays(1, &vertexArrayId);
  glBindVertexArray(vertexArrayId);
  
  static const GLfloat g_vertex_buffer_data[] = 
  {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
  };
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(progId);
  
  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  
  do
  {
    // Put in main loop
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
    glDrawArrays(GL_TRIANGLES, 0 ,3);
    glDisableVertexAttribArray(0);
    glfwSwapBuffers();
    
  } while(glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS && glfwGetWindowParam(GLFW_OPENED));
  
  
  return 0;
}