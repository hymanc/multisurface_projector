cc = g++
ld = g++
cflags = -lm -c
lflags = `pkg-config --cflags --libs opencv` -lm

src = colorTest.cpp colorGenerator.cpp
obj = colorTest.o colorGenerator.o
exe = colorsTest

all: $(exe)

$(exe): $(obj)
	$(ld) $(obj) -o $(exe) $(lflags) 

colorGenerator.o: colorGenerator.cpp
	$(cc) $(cflags) colorGenerator.cpp


colorTest.o: colorTest.cpp colorGenerator.h
	$(cc) $(cflags) colorTest.cpp 
		
	
clean:
	rm -f *.o
