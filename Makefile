CC		:= g++
CFLAGS		:= -I/usr/include/opencv -I/usr/local/include/opencv -L/usr/local/lib
LDFLAGS		:=
LIBRARIES	:= -lopencv_core -lopencv_imgproc -lopencv_highgui
SOURCES		:= autoFocus.cpp
OBJECTS		:= $(SOURCES:.cpp=.o)
EXECUTABLE	:= autoFocus

.PHONY: all clean

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBRARIES) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(LIBRARIES) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
