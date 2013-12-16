cc = g++
ld = g++
cflags = -lm -c
lflags = `pkg-config --cflags --libs opencv` -lm

src = surfaceMapper.cpp ProcGen.cpp ActiveStereoMap.cpp ActiveCalib.cpp Reconstructor.cpp gray_lookup.cpp
obj = surfaceMapper.o ProcGen.o ActiveStereoMap.o ActiveCalib.o Reconstructor.o gray_lookup.o
exe = surfaceMapper

all: $(exe)

$(exe): $(obj)
	$(ld) $(obj) -o $(exe) $(lflags) 

clean:
	rm -f *.o
	rm -f $(exe)
