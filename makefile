#compiler
CC=gcc
OPTS=-c -Wall
CFLAGS=
#source files
ODIR = obj
SDIR = src

ROOT_SOURCES=$(wildcard $(SDIR)/*.c)
SUBF_SOURCES=$(wildcard $(SDIR)/*/*.c)
#object files
OBJECTS=$(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(ROOT_SOURCES) $(SUBF_SOURCES))
#sdl-config or any other library here. 
#``- ensures that the command between them is executed, and the result is put into LIBS
#executable filename
EXECUTABLE=TT_server
#Special symbols used:
#$^ - is all the dependencies (in this case =$(OBJECTS) )
#$@ - is the result name (in this case =$(EXECUTABLE) )

default: clean $(ODIR) all
	
$(ODIR):
	mkdir $@
	mkdir $@/core
	mkdir $@/networks
	mkdir $@/model
	mkdir $@/map
	mkdir $@/game

all: $(EXECUTABLE)

clean:
	rm -rf $(ODIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(ODIR)/*.o $(ODIR)/*/*.o -pthread -o $@

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) 