##
# Artillery Game
#
# @file
# @version 0.1

export PLATFORM=PLATFORM_DESKTOP
export GLFW_LINUX_ENABLE_X11=FALSE
export GLFW_LINUX_ENABLE_WAYLAND=TRUE
export LD_LIBRARY_PATH=/home/cockroach/coding/c/asteroids/shared
export BUILDLIST=( $OBJECTLOGIC $GAMELOGIC $SYSLOGIC )
export WARNINGS=-Wall -Wextra
export RUN_MAIN=1

define buildLib
	@if [ -z $1 ]; then echo "No argument provided"; exit 1; fi
	@if [ -f object-files/$1.o ]; then rm object-files/$1.o; fi
	@if [ -f shared/$1.so ]; then rm shared/$1.so; fi

    clang $(WARNINGS) -g -fPIC -ferror-limit=0 -lraylib -I headers/ -o object-files/$1.o -c src/$1.c
    clang -g -fPIC -ferror-limit=0 -shared -o shared/lib$1.so object-files/$1.o
endef

buildAll: buildObjectLogic buildGameLogic buildSysLogic buildMain

buildObjectLogic:
	$(call buildLib,objectlogic)

buildGameLogic:
	$(call buildLib,gamelogic)

buildSysLogic:
	$(call buildLib,syslogic)

buildMain:
	@if [ -f main.o ]; then rm main.o; fi
	bear -- clang $(WARNINGS) -ferror-limit=0 -g -Og -I headers -Lshared -lobjectlogic -lsyslogic -lgamelogic -lraylib -lGL -lm -lpthread -ldl -lrt -lglfw -o build/main.o src/main.c

run: main.o
	./main.o

# end
