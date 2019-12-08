AUTOMAKE_OPTIONS = foreign



AM_CPPFLAGS = -W  -Wall -g -std=c99
# dynamic library
lib_LTLIBRARIES = libantfx.la
libantfx_la_SOURCES =  supports.c \
			list.c \
			widgets/window.c \
			widgets/image.c \
			widgets/font.c \
			antfx.c

if USE_SDL2
    libantfx_la_SOURCES+=engines/sdl2_engine.c
else
    libantfx_la_SOURCES+=engines/fb_engine.c
endif

pkginclude_HEADERS =  list.h \
            types.h\
			supports.h \
			antfx.h \
			engine.h \
			widgets/widgets.h \
			widgets/window.h \
			widgets/image.h \
			widgets/font.h


EXTRA_DIST = README.md

# bin
bin_PROGRAMS = antfx
# lib source files
if USE_SDL2
    antfx_SOURCES = test/test_supports.c
else
    antfx_SOURCES = antfxapp.c
endif
antfx_LDADD = libantfx.la

if USE_SDL2
    antfx_LDADD += -lSDL2
endif