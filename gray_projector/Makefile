SOURCE = test.cpp

FLAGS = `pkg-config --cflags --libs opencv`


all: $(SOURCE) 
	g++ $(SOURCE) -o test $(FLAGS)

osx: $(SOURCE) 
	g++ $(SOURCE) -o testosx $(FLAGS)

clean: 
	rm -rf test
	
cleanosx: 
	rm -rf testosx

cp:
	echo "Don't do it... Don't you do it"