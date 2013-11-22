SOURCE = colors.cpp

FLAGS = `pkg-config --cflags --libs opencv`


all: $(SOURCE) 
	g++ $(SOURCE) -o colors $(FLAGS)

osx: $(SOURCE) 
	g++ $(SOURCE) -o colorsosx $(FLAGS)

clean: 
	rm -rf colors
	
cleanosx: 
	rm -rf colorsosx