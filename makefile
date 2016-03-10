#compiler
CC=gcc
OPTS=-c -Wall
CFLAGS=
#source files
ODIR = obj
SDIR = src

SOURCES=$(wildcard $(SDIR)/*.c)
GAME_SOURCES=$(wildcard $(SDIR)/game/*.c)
NETW_SOURCES=$(wildcard $(SDIR)/networks/*.c)
#object files
OBJECTS=$(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(SOURCES) $(GAME_SOURCES) $(NETW_SOURCES))
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
	cd $@; mkdir game
	mkdir $@/networks

all: $(EXECUTABLE)

clean:
	rm -f $(ODIR)/*.o

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -pthread -o $@

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) 