# makefile for Snake

CFLAGS  := -Wall
LDFLAGS  := -lcurses
SOURCES := main.c
OBJECTS :=$(SOURCES:.c=.o)
EXECUTABLE := Snake

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f *.o $(EXECUTABLE) *.stackdump
