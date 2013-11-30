SOURCE = shaderTest.cpp ShaderProgram.cpp 

FLAGS = -L /usr/lib -lglfw -lGLEW -lGL

all: $(SOURCE) 
	g++ $(SOURCE) -o shaderTest $(FLAGS)

clean: 
	rm -rf *.o
	rm -rf shaderTest
	
