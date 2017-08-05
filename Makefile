CC=gcc

CFLAGS= -W  -Wall -std=c99 -g -fdiagnostics-color=always  -D DEBUG -D USE_BUFFER

LIB= -lm
BUILD_DIR=./build

OBJS 	= 	supports.o \
			list.o \
			widgets/window.o \
			widgets/image.o \
			widgets/font.o \
			antfx.o

OBJS_SIM =	engines/sdl2_engine.o \
			$(OBJS)

OBJS_FB =	engines/fb_engine.o \
			$(OBJS)

main: sim 

sim:$(OBJS_SIM)
	$(CC) $(CFLAGS)  $(OBJS_SIM) $(LIB)  -lSDL2  -o $(BUILD_DIR)/antfx antfxapp.c
	$(CC) $(CFLAGS)  $(OBJS_SIM) $(LIB)  -lSDL2  -o $(BUILD_DIR)/test_support test/test_supports.c
fb:$(OBJS_FB)
	$(CC) $(CFLAGS)  $(OBJS_FB) $(LIB)  -o $(BUILD_DIR)/antfx antfxapp.c

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

font:
	cd widgets/fonts/ && make

clean:
	-rm -f $(OBJS_SIM) $(OBJS_FB)
	-rm -rf $(BUILD_DIR)/antfx $(BUILD_DIR)/test_support
