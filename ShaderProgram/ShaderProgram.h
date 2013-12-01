#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/gl.h>

using namespace glm;
class ShaderProgram
{
public:
  static GLuint loadShader(const char * vshaderPath, const char * fshaderPath);
private:
  static std::string readShaderSource(const char * sourcePath);
  static bool compileShader(GLuint *shaderId, std::string source, std::string fname);
  static bool linkShaders(GLuint * progId, GLuint vshaderId, GLuint fshaderId);
};