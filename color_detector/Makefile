cc = g++
ld = g++
cflags = -lm -c
lflags = `pkg-config --cflags --libs opencv` -lm

src = color_detect.cpp
obj = color_detect.o
exe = color_detect

all: $(exe)

$(exe): $(obj)
	$(ld) $(obj) -o $(exe) $(lflags) 

color_detect.o: color_detect.cpp
	$(cc) $(cflags) color_detect.cpp

clean:
	rm -f *.o
