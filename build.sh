#!/usr/bin/env sh

export PLATFORM=PLATFORM_DESKTOP
export GLFW_LINUX_ENABLE_X11=FALSE
export GLFW_LINUX_ENABLE_WAYLAND=TRUE
export LD_LIBRARY_PATH=/home/cockroach/coding/c/raylib/asteroids/shared

ASTEROIDS="asteroids"
PLAYERLOGIC="playerlogic"
SYSLOGIC="syslogic"
GAMELOGIC="gamelogic"
MAIN="main.o"

BUILDLIST=( $ASTEROIDS $PLAYERLOGIC $SYSLOGIC $GAMELOGIC )
WARNINGS="-Wall -Wextra"

RUN=1

for i in $BUILDLIST; do

    if [ -z $i ]; then continue; fi
    if [ -f object-files/"$i".o ]; then rm object-files/"$i".o; fi
    if [ -f shared/"$i".so ]; then rm shared/"$i".so; fi

    clang $WARNINGS -fPIC -lraylib -I headers/ -o object-files/"$i".o -c "$i".c
    clang -shared -o shared/lib"$i".so object-files/"$i".o
done

if [ -n $MAIN ]; then
    if [ -f $MAIN ]; then
        rm $MAIN;
    fi
    clang $WARNINGS -g -I headers -Lshared -lplayerlogic -lsyslogic -lgamelogic -lraylib -lasteroids -lGL -lm -lpthread -ldl -lrt -lglfw -o $MAIN main.c
fi

if [ $RUN == 1 ]; then
    ./main.o
fi
