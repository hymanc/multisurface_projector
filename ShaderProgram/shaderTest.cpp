#include "ShaderProgram.h"
#include "controls.hpp"
#include <SOIL/SOIL.h>
#include <GL/glfw.h>

using namespace std;
using namespace glm;

bool initWindow(void)
{
  if(!glfwInit())
  {
    fprintf(stderr, "Error initializing GLFW\n");
    return false;
  }
  glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR,3);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR,3);
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  if(!glfwOpenWindow(1024, 768, 0,0,0,0,32,0,GLFW_WINDOW))
  {
    fprintf(stderr, "Error creating window\n");
    glfwTerminate();
    return false;
  }
  glewExperimental = true;
  if(glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Error initializing GLEW\n");
    return false;
  }
  glfwSetWindowTitle("Shader test");
  glfwEnable(GLFW_STICKY_KEYS);
  return true;
}

GLuint initDefaultShader(void)
{
  const char * vshaderPath = "/home/cody/dev/multisurface_projector/ShaderProgram/vert.glsl";
  const char * fshaderPath = "/home/cody/dev/multisurface_projector/ShaderProgram/frag.glsl";
  return ShaderProgram::loadShader(vshaderPath, fshaderPath);
}

GLuint initPMapShader(void)
{
  const char * vshaderPath = "/home/cody/dev/multisurface_projector/ShaderProgram/proj_vert.glsl";
  const char * fshaderPath = "/home/cody/dev/multisurface_projector/ShaderProgram/proj_frag.glsl";
  return ShaderProgram::loadShader(vshaderPath, fshaderPath);
}


int main(int argc, char ** argv)
{

  if(!initWindow())
    return -1;
  
  //GLuint progId = initDefaultShader();
  GLuint projProgId = initPMapShader();
  
  GLuint vertexArrayId;
  glGenVertexArrays(1, &vertexArrayId);
  glBindVertexArray(vertexArrayId);
  
  GLfloat size = 10.0f;
  static const GLfloat g_vertex_buffer_data[] = 
  {
    -size, -size, 0.0f,
    -size, size, 0.0f,
    size, size, 0.0f,
    size, size, 0.0f,
    -size, -size, 0.0f,
    size, -size, 0.0f
  };

  
  
  // Standard bag of triangles
  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  
  
  /*
  // Indexed
  std::vector<unsigned short> indices;
  GLuint elementbuffer;
  glGenBuffers(1, &elementbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
  */
  
  //printf("Getting matrix uniform IDs\n");
  GLuint matrixId = glGetUniformLocation(projProgId, "MVP");
  GLuint texMatrixId = glGetUniformLocation(projProgId, "texMVP");
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); 
  //glEnable(GL_CULL_FACE);
  
  //GLuint FramebufferName = 0;
  //glGenFramebuffers(1, &FramebufferName);
  //glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

  
   printf("Grabbing texture\n");
  const char* texPath = "home/cody/dev/multisurface_projector/ShaderProgram/checker_256x256.bmp";
  GLuint projTexture = SOIL_load_OGL_texture(
			texPath,
			0,
			SOIL_CREATE_NEW_ID,
			0);
  printf("Texture loaded\n");
  glGenTextures(1, &projTexture);
  glBindTexture(GL_TEXTURE_2D, projTexture);
  printf("Texture bound\n");
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 256,256,0, GL_DEPTH_COMPONENT,GL_FLOAT,0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  
  
  
  /*glDrawBuffer(GL_NONE);
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return -1;
  */

  glUseProgram(projProgId);
  printf("Alive before loop\n");
  do
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Compute transform matrices for camera frame transforms
    computeMatricesFromInputs();
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &MVP[0][0]);
    
    // Projector Frame transforms
    glm::mat4 biasMatrix(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
    );
    glm::mat4 depthModelMatrix = ModelMatrix;
    glm::vec3 synthProjPos(5,0,10);
    glm::mat4 depthProjectionMatrix = glm::perspective(5.0f, 1.0f, 2.0f, 50.0f);
    glm::mat4 depthViewMatrix = glm::lookAt(synthProjPos, vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 texMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
    glUniformMatrix4fv(texMatrixId, 1, GL_FALSE, &texMVP[0][0]);
    
    // First pass for texture mapping
    //glUseProgram(projProgId);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
    glDrawArrays(GL_TRIANGLES, 0 ,6);
    glDisableVertexAttribArray(0);
    
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glViewport(0,0,1024,768); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Second pass
    /*
    glUseProgram(progId);
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &MVP[0][0]);
    // Geometry Drawing for output
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
    glDrawArrays(GL_TRIANGLES, 0 ,6);
    glDisableVertexAttribArray(0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glUniformli(TextureID, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniformli(ProjMapID,1);
    */
    
    // Swap out frame buffer
    glfwSwapBuffers();
    
  } while(glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS && glfwGetWindowParam(GLFW_OPENED));
  
    // Cleanup VBO and shader
  glDeleteBuffers(1, &vertexBuffer);
  //glDeleteProgram(progId);
  //glDeleteTextures(1, &TextureID);
  //glDeleteBuffers(1, &normalbuffer);
  glDeleteVertexArrays(1, &vertexArrayId);
  glDeleteProgram(projProgId);
  glDeleteTextures(1, &projTexture);
  
  //glDeleteFramebuffers(1, &FramebufferName);
  //glDeleteTextures(1, &depthTexture);
  
  // Close OpenGL window and terminate GLFW
  glfwTerminate();
  
  return 0;
}