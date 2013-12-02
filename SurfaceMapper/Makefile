#SOURCE = surfaceMapper.cpp ProcGen.cpp ActiveStereoMap.cpp

#FLAGS = `pkg-config --cflags --libs opencv` -lm


# all: $(SOURCE) 
#	g++ $(SOURCE) -o surfaceMapper $(FLAGS)

# clean: 
#	rm -rf surfaceMapper
	
cc = g++
ld = g++
cflags = -lm -c
lflags = `pkg-config --cflags --libs opencv` -lm

src = surfaceMapper.cpp ProcGen.cpp ActiveStereoMap.cpp
obj = surfaceMapper.o ProcGen.o ActiveStereoMap.o
exe = surfaceMapper

all: $(exe)

$(exe): $(obj)
	$(ld) $(obj) -o $(exe) $(lflags) 

surfaceMapper.o: surfaceMapper.cpp 
	$(cc) $(cflags) surfaceMapper.cpp


procGen.o: procGen.cpp 
	$(cc) $(cflags) procGen.cpp 

ActiveStereoMap.o: ActiveStereoMap.cpp 
	$(cc) $(cflags) ActiveStereoMap.cpp
	
clean:
	rm -f *.o
	rm -f $(exe)
