# initialise autoconf and set up some basic information about the program we’re packaging
AC_INIT([antfx], [1.0.0b], [xsang.le@gmail.com])

# We’re going to use automake for this project
AM_INIT_AUTOMAKE([subdir-objects])

# dependencies
# C compiler
AC_PROG_CC
# libtool for linking
# AC_PROG_LIBTOOL

has_ts=no
# check for lib antd
AC_CHECK_HEADER([tslib.h],[
    has_ts=yes
    # check if the library exists
],[
    AC_MSG_ERROR([Unable to find ts header])
])
AC_CHECK_LIB([ts],[ts_setup],[],[
    if test "$has_ts" = "yes"; then
        AC_MSG_ERROR([Unable to find libts])
    fi
])

# debug option
AC_ARG_ENABLE([debug],
    [  --enable-debug    Turn on debugging],
    [case "${enableval}" in
        yes) AC_DEFINE([DEBUG], [1],[Enable debug]) ;;
        no) ;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
    esac],[debug=false])



use_sdl2=no
AC_CHECK_HEADER([SDL2/SDL.h],[
    use_sdl2=yes
    AC_DEFINE([USE_SDL2], [1],[Enable sdl2])
],[])

AC_CHECK_LIB([SDL2],[SDL_CreateWindow],[],[
    if test "$use_sdl2" = "yes"; then
        AC_MSG_ERROR([Unable to find SDL2 shared lib])
    fi
])

AC_DEFINE([_GNU_SOURCE], [1],[Use GNU source])

AC_CANONICAL_HOST
build_linux=no
build_windows=no
build_mac=no
# Detect the target system
case "${host_os}" in
    linux*)
        AC_DEFINE([LINUX], [1],[Linux system])
        build_linux=yes
        ;;
    darwin*)
        build_mac=yes
        AC_DEFINE([MACOS], [1],[MacOS system])
        ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
        ;;
esac

AC_CANONICAL_HOST

# Pass the conditionals to automake
AM_CONDITIONAL([USE_SDL2], [test "$use_sdl2" = "yes"])
AM_CONDITIONAL([LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([WINDOWS], [test "$build_windows" = "yes"])
AM_CONDITIONAL([OSX], [test "$build_mac" = "yes"])

# find a file called Makefile.in, substitute placeholders
# like @PACKAGE_VERSION@ with values like 0.1.0a, 
# and write the results to Makefile.
AC_CONFIG_FILES([
    Makefile
])

# output the script:
AC_OUTPUT
