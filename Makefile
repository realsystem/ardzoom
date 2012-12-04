CC		:= g++
CFLAGS		:= -I/usr/include/opencv -I/usr/local/include/opencv
LDFLAGS		:= -L/usr/local/lib -L/usr/lib
LIBRARIES	:= -lopencv_core -lopencv_imgproc -lopencv_highgui
SOURCES		:= autoFocus.cpp
OBJECTS		:= $(SOURCES:.cpp=.o)
EXECUTABLE	:= autoFocus

.PHONY: all clean

all: $(SOURCES) $(EXECUTABLE) sock_serv

#tmp
#tmp1

sock_serv: sock_serv.c
	gcc $< -o $@

$(EXECUTABLE): $(SOURCES) 
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LIBRARIES)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) sock_serv
