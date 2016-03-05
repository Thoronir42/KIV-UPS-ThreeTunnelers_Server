#compiler
CC=gcc
#compiler options
OPTS=-c -Wall
#source files
SOURCES=$(wildcard *.c)
#object files
OBJECTS=$(SOURCES:.c=.o)
#sdl-config or any other library here. 
#``- ensures that the command between them is executed, and the result is put into LIBS
#executable filename
EXECUTABLE=TT_server
#Special symbols used:
#$^ - is all the dependencies (in this case =$(OBJECTS) )
#$@ - is the result name (in this case =$(EXECUTABLE) )

default: all clean


all: $(EXECUTABLE)

clean:
	rm $(OBJECTS)

$(EXECUTABLE): root game networks $(OBJECTS)
	$(LINK.o) $^ -pthread -o $@	

root: settings
	$(CC) *.c $(OPTS)

game: settings
	$(CC) game/*.c $(OPTS)
	
networks: settings
	$(CC) networks/*.c $(OPTS)
	
settings:
	$(CC) settings.c $(OPTS)