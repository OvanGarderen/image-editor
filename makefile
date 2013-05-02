CC :=gcc
DBG :=-g
CFLAGS := -c -Wall -Wno-variadic-macros -Wno-unused-parameter -Werror=implicit -Werror=shadow -Wextra -std=gnu11 -Wl,-export-dynamic
INCLUDE := $(shell pkg-config --cflags glib-2.0) \
	$(shell sdl-config --cflags) \
	$(shell pkg-config --cflags libconfig) -I./src/ 
LDFLAGS := $(shell sdl-config --libs) -lSDL_image -lSDL_gfx -pthread \
	$(shell pkg-config --libs glib-2.0) \
	$(shell pkg-config --libs libconfig) \
	-lm -ldl -rdynamic

SOURCES := $(wildcard src/*.c)
_SOURCES := $(patsubst src/%, %, $(SOURCES)) 
OBJECTS := $(patsubst %.c, objects/%.o, $(_SOURCES))
EXECUTABLE=img

LIBSOURCES := $(wildcard src/libsrc/*.c)
_LIBSOURCES := $(patsubst src/libsrc/%, %, $(LIBSOURCES))
SHAREDOBJS := $(patsubst %.c, libs/lib%.so, $(_LIBSOURCES))

.DEPEND: $(SOURCES) $(wildcard src/*.h) $(LIBSOURCES)

all: $(EXECUTABLE) libs

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

libs: $(SHAREDOBJS)

objects/libobjs/%.o: src/libsrc/%.c
	$(CC) $(CFLAGS) $(DBG) -fPIC $(INCLUDE) $< -o $@ $(LDFLAGS)
	@echo -e "\033[32m----------------------------------------------------------\033[0m"

objects/%.o: src/%.c 
	$(CC) $(CFLAGS) $(DBG) $(INCLUDE) $< -o $@ $(LDFLAGS)
	@echo -e "\033[31m----------------------------------------------------------\033[0m"

libs/lib%.so: objects/libobjs/%.o 
	$(CC) -shared -Wl,-soname,$@ $< -o $@ 

run:	all
	./$(EXECUTABLE)

.dummy:
	@echo "wat"

clean:
	rm objects/*.o

cleanlibs:
	rm libs/*.so
	rm objects/libobjs/*.o

restart: clean cleanlibs
	rm $(EXECUTABLE)
