CC=gcc
CFLAGS=-c -Wall -std=c99
INCLUDE=$(shell pkg-config --cflags glib-2.0) $(shell sdl-config --cflags)
LDFLAGS=$(shell sdl-config --libs) -lSDL_image -lSDL_gfx $(shell pkg-config --libs glib-2.0)
#-LSDL -lSDL -lSDL_image -lSDL_gfx -lglib-2.0

SOURCES := $(wildcard src/*.c)
_SOURCES := $(patsubst src/%, %, $(SOURCES)) 
OBJECTS := $(patsubst %.c, objects/%.o, $(_SOURCES))
EXECUTABLE=img

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

objects/%.o: src/%.c
	$(CC) $(CFLAGS) $(DBG) $(INCLUDE) $< -o $@ $(LDFLAGS)

run:	$(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm objects/*.o

restart: clean
	rm $(EXECUTABLE)
