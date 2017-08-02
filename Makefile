CC=gcc

CFLAGS= -W  -Wall -std=c99 -g  -D DEBUG -D USE_BUFFER
LIB= -lSDL2
BUILD_DIR=./build

OBJS 	= 	supports.o \
			antfx.o

OBJS_SIM =	engines/sdl2_engine.o \
			$(OBJS)

OBJS_FB =	engines/fb_engine.o \
			$(OBJS)

main: sim 

sim:$(OBJS_SIM)
	$(CC) $(CFLAGS)  $(OBJS_SIM)  -lSDL2  -o $(BUILD_DIR)/antfx

fb:$(OBJS_FB)
	$(CC) $(CFLAGS)  $(OBJS_FB)   -o $(BUILD_DIR)/antfx

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(OBJS_SIM) $(OBJS_FB)
	-rm -rf $(BUILD_DIR)/*