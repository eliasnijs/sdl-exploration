#!/bin/sh

# IMPORTANT(Elias): Assumes you are on linux!

# files
SOURCE="./game/src/sdl_lng.cpp"
OUTPUT="./build/lupy-niceguy-game"

# includes and libs
INCS=""
LIBS="-lSDL2main -lSDL2"

# flags
DFLAGS=""
CPPFLAGS="-D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\""
#CFLAGS="-g -Wall -Wno-deprecated-declarations -O0 -O2 ${INCS} ${CPPFLAGS}"
CFLAGS="-g -Wall -Wno-deprecated-declarations ${INCS} ${CPPFLAGS}"
LDFLAGS=${LIBS}

# compiler and linker
CC="g++" 

# building
cd $(dirname $0)
${CC} $CFLAGS $DFLAGS -o $OUTPUT $SOURCE $LDFLAGS
