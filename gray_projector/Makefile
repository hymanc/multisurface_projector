SOURCE = grayTest.cpp ProcGen.cpp

FLAGS = `pkg-config --cflags --libs opencv` -lm


all: $(SOURCE) 
	g++ $(SOURCE) -o grayProjector $(FLAGS)

osx: $(SOURCE) 
	g++ $(SOURCE) -o grayProjectorosx $(FLAGS)

clean: 
	rm -rf grayProjector
	
cleanosx: 
	rm -rf grayProjectorosx

cp:
	echo "Don't do it... Don't you do it"