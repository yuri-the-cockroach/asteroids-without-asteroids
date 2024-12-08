##
# Artillery Game
#
# @file
# @version 0.1

export GLFW_LINUX_ENABLE_X11=FALSE
export GLFW_LINUX_ENABLE_WAYLAND=TRUE

export WARNINGS += -Wall
export WARNINGS += -Wextra
export WARNINGS += -Weverything
export WARNINGS += -Wno-unsafe-buffer-usage
export WARNINGS += -Wno-declaration-after-statement
export WARNINGS += -Wno-missing-noreturn
export WARNINGS += -Wno-padded
export WARNINGS += -Wno-switch-default
export WARNINGS += -Wno-double-promotion
export WARNINGS += -Wno-pre-c23-compat

# Optimizations options (will be overriten if debugging is enabled)
export OPTIMIZE=-O2 -g0

export LIBS+= -lunwind
export LIBS+= -llogger
export LIBS+= -lautils
export LIBS+= -lrender
export LIBS+= -lobjectlogic
export LIBS+= -lsyslogic
export LIBS+= -lgamelogic
export LIBS+= -lraylib
export LIBS+= -lobjecthandler
export LIBS+= -lm
export LIBS+= -lpthread
export LIBS+= -ldl
export LIBS+= -lrt
export LIBS+= -lglfw
export LIBS+= -lcollision
export LIBS+= -lstatemachine
export LIBS+= -lmenulogic
export LIBS+= -lasteroid

# Define these to enable debugging and benchmarking respectively
export DEBUGGING = -DDEBUGGING
export BENCHMARKING = -DBENCHMARKING

ifdef DEBUGGING

export LIBS += -lvisdebugger
export OPTIMIZE = -O0 -g2 # Overrides previous optimization options
export SANITIZER += -fsanitize=address
export SANITIZER += -fsanitize-address-use-after-return=always
export SANITIZER += -fno-omit-frame-pointer

endif # DEBUGGING

ifdef BENCHMARKING
export LIBS+= -lbenchmarking
endif # BENCHMARKING


define buildLib
    clang  $(WARNINGS) $(OPTIMIZE) $(SANITIZER) $(DEBUGGING) $(BENCHMARKING) -std=c23 -fPIC -ferror-limit=0 -I src/ -o object-files/$1.o -c src/$1.c
    clang  $(WARNINGS) $(OPTIMIZE) $(SANITIZER) $(DEBUGGING) $(BENCHMARKING) -std=c23 -fPIC -ferror-limit=0 -shared -o shared/lib$1.so object-files/$1.o
endef

all:
	mold -run make -j $(nproc) \
		shared/liblogger.so \
		shared/libautils.so \
		shared/libobjecthandler.so \
		shared/libobjectlogic.so \
		shared/libgamelogic.so \
		shared/libsyslogic.so \
		shared/librender.so \
		shared/libcollision.so \
		shared/libvisdebugger.so \
		shared/libstatemachine.so \
		shared/libmenulogic.so \
		shared/libasteroid.so \
		shared/libbenchmarking.so
	mold -run make main


shared/libmenulogic.so: makefile src/menulogic.h src/menulogic.c
	$(call buildLib,menulogic)

shared/libcollision.so: makefile src/collision.h src/collision.c
	$(call buildLib,collision)

shared/librender.so: makefile src/render.h src/render.c
	$(call buildLib,render)

shared/liblogger.so: makefile src/logger.h src/logger.c
	$(call buildLib,logger)

shared/libautils.so: makefile src/autils.h src/autils.c
	$(call buildLib,autils)

shared/libobjectlogic.so: makefile src/objectlogic.h src/objectlogic.c
	$(call buildLib,objectlogic)

shared/libgamelogic.so: makefile src/gamelogic.h src/gamelogic.c
	$(call buildLib,gamelogic)

shared/libsyslogic.so: makefile src/syslogic.h src/syslogic.c
	$(call buildLib,syslogic)

shared/libobjecthandler.so: makefile src/objecthandler.h src/objecthandler.c
	$(call buildLib,objecthandler)

shared/libcollider.so: makefile src/collider.h src/collider.c
	$(call buildLib,collider)

shared/libvisdebugger.so: makefile src/visdebugger.h src/visdebugger.c
	$(call buildLib,visdebugger)

shared/libstatemachine.so: makefile src/statemachine.h src/statemachine.c
	$(call buildLib,statemachine)

shared/libasteroid.so: makefile src/asteroid.h src/asteroid.c
	$(call buildLib,asteroid)

shared/libbenchmarking.so: makefile src/benchmarking.h src/benchmarking.c
	$(call buildLib,benchmarking)

main: makefile main.c
	clang $(WARNINGS) -std=c23 -ferror-limit=0 -rpath shared $(OPTIMIZE) $(SANITIZER) $(DEBUGGING) $(BENCHMARKING) -o main main.c -Isrc -Lshared $(LIBS)

# -lc level for consol debug output
# -lf level for file debug output
run: main
	./main -lc 0 -lf 7
clean:
	if [ -s object-files ]; then rm object-files/*.o; fi
	if [ -s shared ]; then rm shared/*.so; fi
	if [ -e main ]; then rm main; fi

# end
