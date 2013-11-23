SOURCE = CameraCal.cpp Calibrator.cpp

FLAGS = `pkg-config --cflags --libs opencv`


all: $(SOURCE) 
	g++ $(SOURCE) -o cal $(FLAGS)

osx: $(SOURCE) 
	g++ $(SOURCE) -o calosx $(FLAGS)

clean: 
	rm -rf cal
	
cleanosx: 
	rm -rf calosx
