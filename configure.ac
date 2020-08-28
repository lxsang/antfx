# initialise autoconf and set up some basic information about the program we’re packaging
AC_INIT([antfx], [1.0.0b], [xsang.le@gmail.com])

# We’re going to use automake for this project
AM_INIT_AUTOMAKE([subdir-objects])

# dependencies
# C compiler
AC_PROG_CC
# libtool for linking
# AC_PROG_LIBTOOL

# check for lib antd
AC_CHECK_HEADER([tslib.h],[],[
    AC_MSG_ERROR([Unable to find ts header])
])
AC_CHECK_LIB([ts],[ts_setup],[],[
    AC_MSG_ERROR([Unable to find libts])
])

AC_CHECK_LIB([m],[round],[],[
    AC_MSG_ERROR([Unable to find libts])
])

# check for lib antd
AC_CHECK_HEADER([wiringPiI2C.h],[],[
    AC_MSG_ERROR([Unable to find wiringPi header])
])
AC_CHECK_LIB([wiringPi],[wiringPiI2CSetup],[],[
    AC_MSG_ERROR([Unable to find libwringPi])
])


AC_CHECK_HEADER([curl/curl.h],[],[
    AC_MSG_ERROR([Unable to find curl lib header])
])

AC_CHECK_LIB([curl],[curl_global_init],[],[
        AC_MSG_ERROR([Unable to find libcurl])
])

AC_CHECK_HEADER([pthread.h],[],[
    AC_MSG_ERROR([Unable to find pthread lib header])
])

AC_CHECK_LIB([pthread],[pthread_create],[],[
        AC_MSG_ERROR([Unable to find libpthread])
])

AC_CHECK_HEADER([sqlite3.h],[],[
    AC_MSG_ERROR([Unable to find sqlite3 lib header])
])

AC_CHECK_LIB([sqlite3],[sqlite3_open],[],[
        AC_MSG_ERROR([Unable to find libsqlite3])
])


AC_CHECK_HEADER([mpg123.h],[],[
    AC_MSG_ERROR([Unable to find mpg123 lib header])
])

AC_CHECK_LIB([mpg123],[mpg123_init],[],[
        AC_MSG_ERROR([Unable to find libmpg123])
])

AC_CHECK_HEADER([pulse/error.h],[],[
    AC_MSG_ERROR([Unable to find pulseaudio lib header])
])

AC_CHECK_LIB([pulse],[pa_strerror],[],[
        AC_MSG_ERROR([Unable to find libpulse])
])

AC_CHECK_HEADER([pulse/simple.h],[],[
    AC_MSG_ERROR([Unable to find pulseaudio-simple lib header])
])

AC_CHECK_LIB([pulse-simple],[pa_simple_new],[],[
        AC_MSG_ERROR([Unable to find libpulse-simple])
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
