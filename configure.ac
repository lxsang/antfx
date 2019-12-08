# initialise autoconf and set up some basic information about the program we’re packaging
AC_INIT([antfx], [1.0.0b], [xsang.le@gmail.com])

# We’re going to use automake for this project
AM_INIT_AUTOMAKE([subdir-objects])

# dependencies
# C compiler
AC_PROG_CC
# libtool for linking
AC_PROG_LIBTOOL

# debug option
AC_ARG_ENABLE([debug],
    [  --enable-debug    Turn on debugging],
    [case "${enableval}" in
        yes) AC_DEFINE([DEBUG], [1],[Enable debug]) ;;
        no) ;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
    esac],[debug=false])


# buffer option
AC_ARG_ENABLE([buffer],
    [  --enable-buffer    Turn on buffer],
    [case "${enableval}" in
        yes) AC_DEFINE([USE_BUFFER], [1],[Enable debug]) ;;
        no) ;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
    esac],[debug=false])


use_sdl2=no
AC_ARG_ENABLE([sdl2],
    [  --enable-sdl2  Use sdl2 as engile],
    [case "${enableval}" in
        yes) use_sdl2=yes ;;
        no) ;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
    esac],[debug=false])


if test "$use_sdl2" = "yes"; then
    AC_CHECK_HEADER([SDL2/SDL.h],[
        AC_CHECK_LIB([SDL2],[SDL_CreateWindow],[
            AC_DEFINE([USE_SDL2], [1],[Use SLD2 as backend])
        ],[
            AC_MSG_ERROR([Unable to find SDL2 shared library])
        ])
    ],[
        AC_MSG_ERROR([Unable to find SDL2 headers])
    ])
fi

AC_CANONICAL_HOST

# Pass the conditionals to automake
AM_CONDITIONAL([USE_SDL2], [test "$use_sdl2" = "yes"])

# find a file called Makefile.in, substitute placeholders
# like @PACKAGE_VERSION@ with values like 0.1.0a, 
# and write the results to Makefile.
AC_CONFIG_FILES([Makefile])

# output the script:
AC_OUTPUT
