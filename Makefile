CC              := g++
CFLAGS          := -I/usr/include/opencv
LIBRARIES       := -lopencv_core -lopencv_imgproc -lopencv_highgui

.PHONY: all clean

all: autoFocus

autoFocus:
	$(CC) $(CFLAGS) -o autoFocus autoFocus.cpp $(LIBRARIES)
        
clean:
	rm -f *.o
