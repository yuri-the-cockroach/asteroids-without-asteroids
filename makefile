##
# Artillery Game
#
# @file
# @version 0.1

export PLATFORM=PLATFORM_DESKTOP_RGFW
# export GLFW_LINUX_ENABLE_X11=FALSE
# export GLFW_LINUX_ENABLE_WAYLAND=TRUE
# export LD_LIBRARY_PATH=/home/cockroach/coding/c/asteroids/build
export BUILDLIST=( $OBJECTLOGIC $GAMELOGIC $SYSLOGIC )

export WARNINGS+= -Wall
export WARNINGS+= -Wextra
export WARNINGS+= -Weverything
export WARNINGS+= -Wno-unsafe-buffer-usage
export WARNINGS+= -Wno-declaration-after-statement
export WARNINGS+= -Wno-missing-noreturn
export WARNINGS+= -Wno-padded
export WARNINGS+= -Wno-switch-default
export WARNINGS+= -Wno-double-promotion

export RUN_MAIN=1
export OPTIMIZE=-Og -g
# export OPTIMIZE=-O3
export LIBS+= -lunwind
export LIBS+= -llogger
export LIBS+= -lasteroidsutils
export LIBS+= -lrender
export LIBS+= -lobjectlogic
export LIBS+= -lsyslogic
export LIBS+= -lgamelogic
export LIBS+= -lraylib
export LIBS+= -lobjecthandler
# export LIBS+= -lGL
export LIBS+= -lm
export LIBS+= -lpthread
export LIBS+= -ldl
export LIBS+= -lrt
export LIBS+= -lglfw
export LIBS+= -lcollision
export LIBS+= -lvisdebugger
export LIBS+= -lstatemachine
export LIBS+= -lmenulogic
export LIBS+= -lasteroid
export LIBS+= -lbenchmarking

define buildLib
	@if [ -z $1 ]; then echo "No argument provided"; exit 1; fi
	@if [ -f object-files/$1.o ]; then rm object-files/$1.o; fi
	@if [ -f build/$1.so ]; then rm build/$1.so; fi

    clang $(WARNINGS) $(OPTIMIZE) -std=gnu17 -fPIC -ferror-limit=0 -I src/ -o object-files/$1.o -c src/$1.c
    clang $(WARNINGS) $(OPTIMIZE) -std=gnu17 -fPIC -ferror-limit=0 -shared -o build/lib$1.so object-files/$1.o
endef

buildall:
	mold -run make -j \
		asteroidsutils \
		tracker \
		objectlogic \
		gamelogic \
		syslogic \
		render \
		logger \
		collision \
		visdebugger \
		statemachine \
		menulogic \
		asteroid \
		benchmarking
	mold -run make main

menulogic:
	$(call buildLib,menulogic)

collision:
	$(call buildLib,collision)

render:
	$(call buildLib,render)

logger:
	$(call buildLib,logger)

asteroidsutils:
	$(call buildLib,asteroidsutils)

objectlogic:
	$(call buildLib,objectlogic)

gamelogic:
	$(call buildLib,gamelogic)

syslogic:
	$(call buildLib,syslogic)

tracker:
	$(call buildLib,objecthandler)

collider:
	$(call buildLib,collider)

visdebugger:
	$(call buildLib,visdebugger)

statemachine:
	$(call buildLib,statemachine)

asteroid:
	$(call buildLib,asteroid)

benchmarking:
	$(call buildLib,benchmarking)

main:
	@if [ -f main.o ]; then rm main.o; fi
	bear -- clang $(WARNINGS) -std=gnu17 -ferror-limit=0 -rpath . $(OPTIMIZE) -Isrc -o object-files/main.o -c main.c -Lbuild $(LIBS)
	clang $(WARNINGS) -std=gnu17 -ferror-limit=0 -rpath . $(OPTIMIZE) -o build/main object-files/main.o -Lbuild $(LIBS)

# -lc level for consol debug output
# -lf level for file debug output
# -d  debug mode
# -b  breakpoint mode ( press b to toggle GDB_BREAK variable )
run: main.o
	cd build && ./main -b -lc 0 -lf 7 -d

# end
