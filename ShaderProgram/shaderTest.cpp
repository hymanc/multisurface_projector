#include "ShaderProgram.h"
#include "controls.hpp"
#include "objloader.hpp"
#include "vboindexer.hpp"
#include "texture.hpp"

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
  
  GLuint projProgId = initPMapShader();
  
  GLuint vertexArrayId;
  glGenVertexArrays(1, &vertexArrayId);
  glBindVertexArray(vertexArrayId);
  
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	printf("Loading object\n");
	//bool res = loadOBJ("room_thickwalls.obj", vertices, uvs, normals);
	bool res = loadOBJ("blender_mesh2.obj", vertices, uvs, normals);
	
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	printf("Indexing VBOs\n");
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	printf("Binding vertex buffer\n");
  	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	printf("Binding UV buffer\n");
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	printf("Binding normal buffer\n");
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);



  //printf("Getting matrix uniform IDs\n");
  GLuint matrixId = glGetUniformLocation(projProgId, "MVP");
  GLuint texMatrixId = glGetUniformLocation(projProgId, "texMVP");
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); 
  
  // Load Texture
  //GLuint Texture = loadDDS("uvmap.DDS");
  printf("Grabbing texture\n");
  /*const char *texPath = "home/cody/dev/multisurface_projector/ShaderProgram/checker_256x256.jpg";
  GLuint Texture = SOIL_load_OGL_texture(
			texPath,
			0,
			SOIL_CREATE_NEW_ID,
			0);
  printf("Texture loaded\n");*/
  //GLuint Texture = loadBMP_custom("checker_256x256.bmp");
  GLuint Texture  = loadBMP_custom("chucknorris.bmp");
  //glGenTextures(1, &Texture);
  //glBindTexture(GL_TEXTURE_2D, Texture);
  printf("Texture bound\n");
 
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 512,512,3, GL_DEPTH_COMPONENT,GL_FLOAT,0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  
  // Bind texture
  GLuint TextureID  = glGetUniformLocation(projProgId, "projTexture");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glUniform1i(TextureID, 0);
  
  glUseProgram(projProgId); // Set shader program
  
  // Render loop
  printf("Starting GL Loop\n");
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
    
    glm::mat4 ProjModelMatrix = ModelMatrix;
    //glm::vec3 synthProjPos(5,2,3);
    //glm::mat4 ProjProjectionMatrix = glm::perspective(5.0f, 1.0f, 2.0f, 50.0f);
    //glm::mat4 ProjViewMatrix = glm::lookAt(synthProjPos, vec3(0,0,0), glm::vec3(0,1,0));
    computeProjMatricesFromInputs();
    glm::mat4 ProjViewMatrix = getProjViewMatrix();
    glm::mat4 ProjProjectionMatrix = getProjProjectionMatrix();
    glm::mat4 texMVP = ProjProjectionMatrix * ProjViewMatrix * ProjModelMatrix;
    glUniformMatrix4fv(texMatrixId, 1, GL_FALSE, &texMVP[0][0]);
   
    
      //glEnable(GL_CULL_FACE);
      //glCullFace(GL_BACK);
	
      // Buffer binding
      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
      glVertexAttribPointer(
	      0,  // The attribute we want to configure
	      3,                  // size
	      GL_FLOAT,           // type
	      GL_FALSE,           // normalized?
	      0,                  // stride
	      (void*)0            // array buffer offset
      );

      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
      glVertexAttribPointer(
	1,
	2,
	GL_FLOAT,
	GL_FALSE,
	0,
	(void *) 0
      );
      
      // Index buffer
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

      // Draw the triangles !
      glDrawElements(
	      GL_TRIANGLES,      // mode
	      indices.size(),    // count
	      GL_UNSIGNED_SHORT, // type
	      (void*)0           // element array buffer offset
      );

      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
    
    // Texture binding
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glUniform1i(TextureID, 0);

    glfwSwapBuffers();     // Swap out frame buffer
    
  } while(glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS && glfwGetWindowParam(GLFW_OPENED));
  
    // Cleanup VBO and shader
  glDeleteTextures(1, &TextureID);
  glDeleteBuffers(1, &normalbuffer);
  glDeleteVertexArrays(1, &vertexArrayId);
  glDeleteProgram(projProgId);
  //glDeleteTextures(1, &projTexture);
  glDeleteTextures(1, &Texture);
  
  glfwTerminate(); // Close OpenGL window and terminate GLFW
  
  return 0; // Exit
}