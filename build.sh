#!/bin/sh

# files
SOURCE="./engine/src/tve.c"
OUTPUT="./build/tiny-vulkan-engine"

# includes and libs
INCS=""
LIBS="-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi"

# flags
DFLAGS="-DLVE_INTERNAL -DLVE_SLOW"
CPPFLAGS="-D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\""
CFLAGS="-g -Wall -Wno-deprecated-declarations -O0 -O2 ${INCS} ${CPPFLAGS}"
LDFLAGS=${LIBS}

# compiler and linker
CC="g++" 

# building
cd $(dirname $0)
${CC} $CFLAGS $DFLAGS -o $OUTPUT $SOURCE $LDFLAGS
