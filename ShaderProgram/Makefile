SOURCE = shaderTest.cpp ShaderProgram.cpp controls.cpp

FLAGS = -L /usr/lib -lglfw -lGLEW -lGL

all: $(SOURCE) 
	g++ $(SOURCE) -o shaderTest $(FLAGS)

clean: 
	rm -rf *.o
	rm -rf shaderTest
	
