##
# Artillery Game
#
# @file
# @version 0.1

export PLATFORM=PLATFORM_DESKTOP
export GLFW_LINUX_ENABLE_X11=FALSE
export GLFW_LINUX_ENABLE_WAYLAND=TRUE
export LD_LIBRARY_PATH=/home/cockroach/coding/c/raylib/asteroids/shared

run: build-full
	./main

build-full: clean-full build-syslogic build-gamelogic build-playerlogic build-main

build-playerlogic:
	clang -Wall -fPIC -lraylib -I headers/ -o object-files/playerlogic.o -c playerlogic.c
	clang -shared -o shared/libplayerlogic.so object-files/playerlogic.o

build-gamelogic:
	clang -Wall -fPIC -lraylib -I headers/ -o object-files/gamelogic.o -c gamelogic.c
	clang -shared -o shared/libgamelogic.so object-files/gamelogic.o

build-syslogic:
	clang -Wall -fPIC -lraylib -I headers/ -o object-files/syslogic.o -c syslogic.c
	clang -shared -o shared/libsyslogic.so object-files/syslogic.o

build-main:
	clang -Wall -g -I headers -Lshared -lplayerlogic -lsyslogic -lgamelogic -lraylib -lGL -lm -lpthread -ldl -lrt -lglfw -o main main.c

clean-full:
	@if [ -f main ]; then\
		rm main;\
	fi
	rm shared/* object-files/*

# end
