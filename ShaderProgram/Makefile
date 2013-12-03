	
# The Magical Makefile
# (C) 1982, 
# just kidding
# For Self-Calibrating Projector Project

cc = g++
ld = g++
cflags = -L /usr/lib -lglfw -lGLEW -lGL -lSOIL
lflags = -L /usr/lib -lglfw -lGLEW -lGL -lSOIL

src = shaderTest.cpp ShaderProgram.cpp controls.cpp texture.cpp vboindexer.cpp objloader.cpp
obj = shaderTest.o ShaderProgram.o controls.o vboindexer.o objloader.o texture.o
exe = shaderTest

all: $(exe)

$(exe): $(obj)
	$(ld) $(obj) -o $(exe) $(lflags) 

compile: 
%.o: %.c
	$(cc) -c $(cflags) $*.c -o $*.o
	
clean:
	rm -f *.o
	rm -f $(exe)

