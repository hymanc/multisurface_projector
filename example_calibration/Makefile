SOURCE = camera_calibration.cpp

FLAGS = `pkg-config --cflags --libs opencv`


all: $(SOURCE) 
	g++ $(SOURCE) -o camera_calibration $(FLAGS)

osx: $(SOURCE) 
	g++ $(SOURCE) -o camera_calibration_osx $(FLAGS)

clean: 
	rm -rf camera_calibration out_camera_data.xml
	
cleanosx: 
	rm -rf camera_calibration_osx out_camera_data.xml