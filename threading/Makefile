SOURCE = threadTest.cpp

LPATH = /usr/lib/
LLIBS = -lboost_thread
FLAGS = `pkg-config --cflags --libs opencv` -L$(LPATH) $(LLIBS)


all: $(SOURCE) 
	g++ $(SOURCE) -o threadTest $(FLAGS)

osx: $(SOURCE) 
	g++ $(SOURCE) -o testosx $(FLAGS)

clean: 
	rm -rf threadTest
	
cleanosx: 
	rm -rf testosx

cp:
	echo "Don't do it... Don't you do it"
