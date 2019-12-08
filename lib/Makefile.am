EXTRA_DIST = lua-5.3.4/*.c lua-5.3.4/*.h lua-5.3.4/Makefile

if OSX
all:
	cd lua-5.3.4 && make macosx
else
all:
	cd lua-5.3.4 && make linux
endif


clean-local:
	cd lua-5.3.4 && make clean