##
# Artillery Game
#
# @file
# @version 0.1
ifndef DEFINES

export GLFW_LINUX_ENABLE_X11=FALSE
export GLFW_LINUX_ENABLE_WAYLAND=TRUE

# WARN to SHOW
export WARN += -Wall
export WARN += -Wextra
export WARN += -Weverything

# WARN to IGNORE
export NOWARN += -Wno-unsafe-buffer-usage
export NOWARN += -Wno-declaration-after-statement
export NOWARN += -Wno-missing-noreturn
export NOWARN += -Wno-padded
export NOWARN += -Wno-switch-default
export NOWARN += -Wno-double-promotion
export NOWARN += -Wno-pre-c23-compat
export NOWARN += -Wno-gnu-statement-expression-from-macro-expansion
export LIBS += -lunwind
export LIBS += -llogger
export LIBS += -lautils
export LIBS += -lrender
export LIBS += -lobjectlogic
export LIBS += -lsyslogic
export LIBS += -lgamelogic
export LIBS += -lraylib
export LIBS += -lobjecthandler
export LIBS += -lm
export LIBS += -lpthread
export LIBS += -ldl
export LIBS += -lrt
export LIBS += -lglfw
export LIBS += -lcollision
export LIBS += -lstatemachine
export LIBS += -lmenulogic
export LIBS += -lasteroid

# Define these to enable debugging and benchmarking respectively
# SANITIZE will be read from user env
ifdef SANITIZE
export SANITIZE = -fsanitize=address -fsanitize-address-use-after-return=always -fno-omit-frame-pointer
endif # SANITIZE

# DEBUGGING will be read from user env
ifdef DEBUG
export DEBUGGING = -DDEBUGGING
export LIBS += -lvisdebugger

ifndef OPTIMIZE
export OPTIMIZE=-Og -g # Overrides previous optimization options
endif # OPTIMIZE

endif # DEBUGGING

# Optimizations options (will be overriten if debugging is enabled or passed as env)
ifndef OPTIMIZE
export OPTIMIZE=-O2 -g0
endif

# BENCHMARKING will be read from user env
ifdef BENCH
export BENCHMARKING = -DBENCHMARKING
export LIBS+= -lbenchmarking
endif # BENCHMARKING

define buildLib
    clang $(WARN) $(NOWARN) $(OPTIMIZE) $(SANITIZE) $(DEBUGGING) $(BENCHMARKING) -std=c23 -fPIC -ferror-limit=0 -o object/$1.o -c src/$1.c
    clang $(WARN) $(NOWARN) $(OPTIMIZE) $(SANITIZE) $(DEBUGGING) $(BENCHMARKING) -std=c23 -fPIC -ferror-limit=0 -shared -o shared/lib$1.so object/$1.o
endef

endif #DEFINES

all:
	if [[ ! -e object ]]; then mkdir object; fi
	if [[ ! -e shared ]]; then mkdir shared; fi
	mold -run make -j $(nproc) \
		shared/liblogger.so \
		shared/libautils.so \
		shared/libobjecthandler.so \
		shared/libobjectlogic.so \
		shared/libgamelogic.so \
		shared/libsyslogic.so \
		shared/librender.so \
		shared/libcollision.so \
		shared/libstatemachine.so \
		shared/libmenulogic.so \
		shared/libasteroid.so \
		shared/libbenchmarking.so

ifdef DEBUG
	mold -run make shared/libvisdebugger.so
endif # DEBUG

ifdef BENCH
	mold -run make benchmark
endif

	mold -run make main
	mold -run make unit-tests

shared/libmenulogic.so: makefile src/menulogic.h src/menulogic.c src/structs.h
	$(call buildLib,menulogic)

shared/libcollision.so: makefile src/collision.h src/collision.c src/structs.h
	$(call buildLib,collision)

shared/librender.so: makefile src/render.h src/render.c src/structs.h
	$(call buildLib,render)

shared/liblogger.so: makefile src/logger.h src/logger.c src/structs.h
	$(call buildLib,logger)

shared/libautils.so: makefile src/autils.h src/autils.c src/structs.h
	$(call buildLib,autils)

shared/libobjectlogic.so: makefile src/objectlogic.h src/objectlogic.c src/structs.h
	$(call buildLib,objectlogic)

shared/libgamelogic.so: makefile src/gamelogic.h src/gamelogic.c src/structs.h
	$(call buildLib,gamelogic)

shared/libsyslogic.so: makefile src/syslogic.h src/syslogic.c src/structs.h
	$(call buildLib,syslogic)

shared/libobjecthandler.so: makefile src/objecthandler.h src/objecthandler.c src/structs.h
	$(call buildLib,objecthandler)

shared/libcollider.so: makefile src/collider.h src/collider.c src/structs.h
	$(call buildLib,collider)

shared/libvisdebugger.so: makefile src/visdebugger.h src/visdebugger.c src/structs.h
	$(call buildLib,visdebugger)

shared/libstatemachine.so: makefile src/statemachine.h src/statemachine.c src/structs.h
	$(call buildLib,statemachine)

shared/libasteroid.so: makefile src/asteroid.h src/asteroid.c src/structs.h
	$(call buildLib,asteroid)

shared/libbenchmarking.so: makefile src/benchmarking.h src/benchmarking.c src/structs.h
	$(call buildLib,benchmarking)

unit-tests: run-unit-tests.c makefile src/unit-tests.c src/unit-tests.h src/structs.h
	bear -- clang $(WARN) $(NOWARN) -std=c23 -ferror-limit=0 -rpath shared $(OPTIMIZE) $(SANITIZE) $(DEBUGGING) $(BENCHMARKING) -o unit-tests run-unit-tests.c src/unit-tests.c -Isrc -Lshared $(LIBS)

main: makefile main.c src/structs.h
	bear -- clang $(WARN) $(NOWARN) -std=c23 -ferror-limit=0 -rpath shared $(OPTIMIZE) $(SANITIZE) $(DEBUGGING) $(BENCHMARKING) -o main main.c -Isrc -Lshared $(LIBS)

benchmark: benchmark.c makefile src/structs.h
	bear -- clang $(WARN) $(NOWARN) -std=c23 -ferror-limit=0 -rpath shared $(OPTIMIZE) $(SANITIZE) $(DEBUGGING) $(BENCHMARKING) -o benchmark benchmark.c -Isrc -Lshared $(LIBS)

# -lc level for consol debug output
# -lf level for file debug output

run-unit: unit-tests
	./unit-tests -lc 7 -lf 7

run: main
	./main -lc 2 -lf 7

run-bench: benchmark
	./benchmark -lc 4 -lf 7

run-prof: main
	gprofng collect app -O test.er ./main
	gprofng display text -fsummary test.er

clean:
	if [[ -e object && -s object ]]; then rm object/*.o; fi
	if [[ -e shared && -s shared ]]; then rm shared/*.so; fi
	if [ -e main ]; then rm main; fi
	if [ -e unit-tests ]; then rm unit-tests; fi
	if [ -e benchmark ]; then rm benchmark; fi

debug:
	gdb -i=mi main

# end
