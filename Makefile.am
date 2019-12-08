AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -W  -Wall -g -std=c99

SUBDIRS =  lib/ .

# dynamic library
lib_LTLIBRARIES = libantfx.la
libantfx_la_SOURCES =  supports.c \
			list.c \
			widgets/window.c \
			widgets/image.c \
			widgets/font.c \
			antfx.c

if USE_SDL2
    libantfx_la_SOURCES+=backends/sdl2_engine.c
else
    libantfx_la_SOURCES+=backends/fb_engine.c
endif

pkginclude_HEADERS =  list.h \
            types.h\
			supports.h \
			antfx.h \
			backend.h \
			widgets/widgets.h \
			widgets/window.h \
			widgets/image.h \
			widgets/font.h


EXTRA_DIST = README.md

# bin
bin_PROGRAMS = antfx antfly
# lib source files
antfx_SOURCES = antfxapp.c
antfx_LDADD = libantfx.la

if USE_SDL2
    antfx_LDADD += -lSDL2
endif


antfly_SOURCES = antfly.c
antfly_LDADD = libantfx.la lib/lua-5.3.4/liblua.a

if USE_SDL2
    antfly_LDADD += -lSDL2
endif