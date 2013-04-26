CC :=gcc
DBG :=-g
CFLAGS := -c -Wall -Wno-variadic-macros -Wno-unused-parameter -Werror=implicit -Werror=shadow -Wextra -std=gnu11
INCLUDE := $(shell pkg-config --cflags glib-2.0) \
	$(shell sdl-config --cflags) \
	$(shell pkg-config --cflags libconfig) -I./src/ 
LDFLAGS := $(shell sdl-config --libs) -lSDL_image -lSDL_gfx -pthread \
	$(shell pkg-config --libs glib-2.0) \
	$(shell pkg-config --libs libconfig)

SOURCES := $(wildcard src/*.c)
_SOURCES := $(patsubst src/%, %, $(SOURCES)) 
OBJECTS := $(patsubst %.c, objects/%.o, $(_SOURCES))
EXECUTABLE=img

LIBSOURCES := $(wildcard src/libsrc/*.c)
_LIBSOURCES := $(patsubst src/libsrc/%, %, $(LIBSOURCES))
SHAREDOBJS := $(patsubst %.c, libs/lib%.so, $(_LIBSOURCES))

all: $(EXECUTABLE) libs

.depend: $(SOURCES) $(wildcard src/*.h) $(LIBSOURCES)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

libs: libs/libapi.so $(SHAREDOBJS)

objects/dyn_loader.o: src/dyn_loader.c
	$(CC) $(CFLAGS) $(DBG) $(INCLUDE) $< -o $@ $(LDFLAGS) -pthread

objects/api/%.o: src/%.c
	$(CC) $(CFLAGS) $(DBG) -fPIC $(INCLUDE) $< -o $@ $(LDFLAGS)
	@echo -e "\033[33m----------------------------------------------------------\033[0m"

objects/libobjs/%.o: src/libsrc/%.c
	$(CC) $(CFLAGS) $(DBG) -fPIC $(INCLUDE) $< -o $@ $(LDFLAGS)
	@echo -e "\033[32m----------------------------------------------------------\033[0m"

objects/%.o: src/%.c 
	$(CC) $(CFLAGS) $(DBG) $(INCLUDE) $< -o $@ $(LDFLAGS)
	@echo -e "\033[31m----------------------------------------------------------\033[0m"

libs/libapi.so: $(patsubst objects/%.o, objects/api/%.o, $(OBJECTS))
	$(CC) -shared -Wl,-soname,libs/libapi.so -o libs/libapi.so $^

libs/libmode_select.so: objects/libobjs/mode_select.o libs/libapi.so
	$(CC) -shared -Wl,-soname,$@ $< -o $@ -Llibs -lapi

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
