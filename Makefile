CC              := g++
CFLAGS          := -I/usr/local/include/opencv -L/usr/local/lib
OBJECTS         := 
LIBRARIES       := -lopencv_core -lopencv_imgproc -lopencv_highgui

.PHONY: all clean

all: autoFocus

autoFocus:
	$(CC) $(CFLAGS) -o autoFocus autoFocus.cpp $(LIBRARIES)
        
clean:
	rm -f *.o
