#include "ShaderProgram.h"
GLuint ShaderProgram::loadShader(const char * vshaderPath, const char * fshaderPath)
{
  std::cout << "Creating shader IDs\n";
  GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  std::cout << "Vertex shader ID created\n";
  GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  std::cout << "Fragment shader ID create\n";
  
  // Read in source to string buffers
  std::cout << "Reading in shader source from files\n";
  std::string vertexSource;
  std::string fragmentSource;
  vertexSource   = readShaderSource(vshaderPath);
  fragmentSource = readShaderSource(fshaderPath);
  if(vertexSource.empty())
  {
    std::cout << "Error: vertex shader source is empty\n";
    std::cout << "Program will now exit\n" << "FIX THE PROBLEM!\n";
    exit(1);
  }
  if(fragmentSource.empty())
  {
    std::cout << "Error: fragment shader source is empty\n";
    std::cout << "Program will now exit\n" << "FIX THE PROBLEM!\n";
    exit(1);
  }
  // Compile shaders
  std::cout << "Compiling shaders\n";
  if(!compileShader(&vertexShaderId, vertexSource, vshaderPath))
    return 0;
  if(!compileShader(&fragmentShaderId, fragmentSource, fshaderPath))
    return 0;
  
  // Link shaders
  std::cout << "Linking shaders\n";
  GLuint progId;
  if(!linkShaders(&progId, vertexShaderId, fragmentShaderId))
    return 0;
  
  std::cout << "Cleaning shader objects\n";
  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);
  
  return progId;
}

/**
 * @brief Reader for shader source code files
 */
std::string ShaderProgram::readShaderSource(const char * sourcePath)
{
  std::string shaderId;
  std::ifstream sourceStream(sourcePath, std::ios::in);
  if(sourceStream.is_open())
  {
    std::string line = "";
    while(getline(sourceStream, line))
      shaderId += "\n" + line;
    sourceStream.close();
  }
  return shaderId;
}

/**
 * @brief GLSL shader compiler convenience function
 */
bool ShaderProgram::compileShader(GLuint *shaderId, std::string source, std::string fname)
{
  std::cout << "Compiling shader\n" << fname << std::endl;
  char const * sourcePtr = source.c_str();
  glShaderSource(*shaderId, 1, &sourcePtr, NULL);
  glCompileShader(*shaderId);
  // Check compilation status
  GLint result = GL_FALSE;
  int infoLogLength;
  glGetShaderiv(*shaderId, GL_COMPILE_STATUS, &result);
  glGetShaderiv(*shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
  std::vector<char> shaderErrMsg(infoLogLength);
  glGetShaderInfoLog(*shaderId, infoLogLength, NULL, &shaderErrMsg[0]);
  if(result == GL_FALSE)
  {
    fprintf(stderr,"%s\n", &shaderErrMsg[0]);
    return false;
  }
  return true;
}

/** 
 * @brief Simple shader linker
 */
bool ShaderProgram::linkShaders(GLuint * progId, GLuint vshaderId, GLuint fshaderId)
{
  std::cout << "Linking shaders\n";
  *progId = glCreateProgram();
  glAttachShader(*progId, vshaderId);
  glAttachShader(*progId, fshaderId);
  glLinkProgram(*progId);
  
  // Check link status
  GLint result = GL_FALSE;
  int infoLogLength;
  glGetProgramiv(*progId, GL_LINK_STATUS, &result);
  glGetProgramiv(*progId, GL_INFO_LOG_LENGTH, &infoLogLength);
  std::vector<char> progErrMsg(max(infoLogLength, int(1)));
  glGetProgramInfoLog(*progId, infoLogLength, NULL, &progErrMsg[0]);
  if(result == GL_FALSE)
  {
    fprintf(stderr, "%s\n", &progErrMsg[0]);
    return false;
  }
  return true;
}
