##
# Artillery Game
#
# @file
# @version 0.1

export PLATFORM=PLATFORM_DESKTOP
export GLFW_LINUX_ENABLE_X11=FALSE
export GLFW_LINUX_ENABLE_WAYLAND=TRUE
export LD_LIBRARY_PATH=/home/cockroach/coding/c/asteroids/build
export BUILDLIST=( $OBJECTLOGIC $GAMELOGIC $SYSLOGIC )
export WARNINGS=-Wall -Wextra
export RUN_MAIN=1

define buildLib
	@if [ -z $1 ]; then echo "No argument provided"; exit 1; fi
	@if [ -f object-files/$1.o ]; then rm object-files/$1.o; fi
	@if [ -f build/$1.so ]; then rm build/$1.so; fi

    clang $(WARNINGS) -g -fPIC -ferror-limit=0 -lraylib -I src/ -o object-files/$1.o -c src/$1.c
    clang -g -fPIC -ferror-limit=0 -shared -o build/lib$1.so object-files/$1.o
endef

buildAll:buildObjectHandler buildObjectLogic buildGameLogic buildSysLogic buildMain

buildObjectLogic:
	$(call buildLib,objectlogic)

buildGameLogic:
	$(call buildLib,gamelogic)

buildSysLogic:
	$(call buildLib,syslogic)

buildObjectHandler:
	$(call buildLib,objecthandler)

buildMain:
	@if [ -f main.o ]; then rm main.o; fi
	bear -- clang $(WARNINGS) -ferror-limit=0 -g -Og -Isrc -Lbuild -lobjectlogic -lsyslogic -lgamelogic -lraylib -lobjecthandler -lGL -lm -lpthread -ldl -lrt -lglfw -o build/main.o main.c

run: main.o
	./build/main.o

# end
