#SOURCE = surfaceMapper.cpp ProcGen.cpp ActiveStereoMap.cpp

#FLAGS = `pkg-config --cflags --libs opencv` -lm


# all: $(SOURCE) 
#	g++ $(SOURCE) -o surfaceMapper $(FLAGS)

# clean: 
#	rm -rf surfaceMapper
	
cc = g++
ld = g++
cflags = -lm -c `pkg-config --cflags --libs opencv`
lflags = `pkg-config --cflags --libs opencv` -lm

src = match.cpp
obj = match.o
exe = stereoMatch

all: $(exe)

$(exe): $(obj)
	$(ld) $(obj) -o $(exe) $(lflags) 

match.o: match.cpp 
	$(cc) $(cflags) match.cpp
	
clean:
	rm -f *.o
	rm -f $(exe)
