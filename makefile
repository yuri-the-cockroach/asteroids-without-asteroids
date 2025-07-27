##
# Artillery Game
#
# @file
# @version 0.1
ifndef DEFINES

export DISPLAY?=:0

LOGSHELL?=5
LOGFILE?=9

# WARN to SHOW
export WARN += -Wall
export WARN += -Wextra

# WARN to IGNORE
export NOWARN += -Wno-unsafe-buffer-usage
export NOWARN += -Wno-declaration-after-statement
export NOWARN += -Wno-missing-noreturn
export NOWARN += -Wno-padded
export NOWARN += -Wno-switch-default
export NOWARN += -Wno-double-promotion
export NOWARN += -Wno-pre-c23-compat
export NOWARN += -Wno-gnu-statement-expression-from-macro-expansion
export NOWARN += -Wno-c++98-compat
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
export LIBS += -lmt
export LIBS += -lunwind -ldw -lBlocksRuntime
export STATIC
# Define these to enable debugging and benchmarking respectively
# SANITIZE will be read from user env

# DEBUGGING will be read from user env
ifdef DEBUG
export DEBUGGING = -DDEBUGGING
export LIBS += -lvisdebugger

ifndef OPTIMIZE
export OPTIMIZE=-Og -ggdb3 # Overrides previous optimization options
endif # OPTIMIZE

endif # DEBUGGING

ifdef SANITIZE
export SANITIZE = -fsanitize=address -fsanitize-address-use-after-return=always -fno-omit-frame-pointer
endif # SANITIZE

# Optimizations options (will be overriten if debugging is enabled or passed as env)
ifndef OPTIMIZE
export OPTIMIZE=-O2 -g0
endif

export CFLAGS=-D_GNU_SOURCE -mavx2 -std=gnu23 -rdynamic -fblocks

# BENCHMARKING will be read from user env
ifdef BENCH
export BENCHMARKING = -DBENCHMARKING
endif # BENCHMARKING
TARGETS=shared \
		object \
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
		shared/libmt.so

ifdef DEBUG
	TARGETS+= shared/libvisdebugger.so
endif
ifdef BENCH
	TARGETS+= shared/libbenchmarking.so
endif

endif #DEFINES

.PHONY: all clean

.DEFAULT: all main

all: $(TARGETS)

object:
	mkdir object

shared:
	mkdir shared


shared/lib%.so: src/%.c src/%.h makefile src/structs.h
	clang $(CFLAGS) $(WARN) $(NOWARN) $(OPTIMIZE) $(SANITIZE) $(DEBUGGING) $(BENCHMARKING) -fPIC -ferror-limit=0 -shared -o $@ $<

unit-tests: run-unit-tests.c makefile src/unit-tests.c src/structs.h
	bear -- clang $(CFLAGS) $(WARN) $(NOWARN) -ferror-limit=0 -rpath shared $(OPTIMIZE) $(SANITIZE) $(DEBUGGING) -o unit-tests $(STATIC) run-unit-tests.c -Isrc -Lshared $(LIBS)

main: makefile main.c src/structs.h
	bear -- clang $(CFLAGS) $(WARN) $(NOWARN) -std=c23 -ferror-limit=0 -rpath shared $(OPTIMIZE) $(SANITIZE) $(DEBUGGING) $(BENCHMARKING) -o main main.c -Isrc -Lshared $(LIBS)

benchmark: benchmark.c makefile src/structs.h
	bear -- clang $(CFLAGS) $(WARN) $(NOWARN) -std=c23 -ferror-limit=0 -rpath shared $(OPTIMIZE) $(SANITIZE) $(DEBUGGING) $(BENCHMARKING) -o benchmark benchmark.c -Isrc -Lshared $(LIBS)

# -lc level for consol debug output
# -lf level for file debug output

run-unit: unit-tests
	./unit-tests -lc $(LOGSHELL) -lf $(LOGFILE)

run: main
	./main -lc $(LOGSHELL) -lf $(LOGFILE)

run-bench: benchmark
	./benchmark -lc $(LOGSHELL) -lf $(LOGFILE)

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
