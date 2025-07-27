// Requires libraries
// -lunwind -ldw -lBlocksRuntime
// And flags
// -rdynamic -fblocks -std=gnu23

#include <dlfcn.h>
#include <errno.h>
#include <error.h>
#include <execinfo.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef TRACINGTOOLS_H_
    #define TRACINGTOOLS_H_

    // Throwing errors
    /* FORMAT(expr) expands to the right "%…"-string for expr’s type */
    #define FORMAT(x)                   \
        _Generic((x),                   \
            signed char: "%hhd",        \
            short: "%hd",               \
            int: "%d",                  \
            long: "%ld",                \
            long long: "%lld",          \
            unsigned char: "%hhu",      \
            unsigned short: "%hu",      \
            unsigned int: "%u",         \
            unsigned long: "%lu",       \
            unsigned long long: "%llu", \
            float: "%f",                \
            double: "%f",               \
            long double: "%Lf",         \
            char: "%c",                 \
            char *: "%s",               \
            const char *: "%s",         \
            default: "%p")

    #ifndef TRACE
        #define fatal(errno_code, status, ...)          \
            {                                           \
                print_stacktrace();                     \
                error(errno_code, status, __VA_ARGS__); \
            }
    #else // TRACE
        #define fatal(errno_code, status, ...)                              \
            {                                                               \
                print_stacktrace();                                         \
                fprintf(stderr, "errno %d status %d ", errno_code, status); \
                fprintf(stderr, __VA_ARGS__);                               \
            }
    #endif // TRACE

    #ifndef UNUSED
        #define UNUSED(x) (void)(x)
    #endif

    // Stack tracing

    // Assumes 16kB stack size. That should be enough (I really hope so, I DO
    // NOT WANT to malloc stack)
    #define MAX_STACK_DEPTH 2048u
    // An abstract value that will hopefuly be enough
    #define MAX_CMD_LEN 512u
    // Make sure any name will fit with ./ prefix
    #define MAX_BIN_NAME_LEN NAME_MAX + 2

int print_stacktrace(void) {

    int temp_errno = errno; // remember the state of errno, because `backtrace`
                            // func for some reason sets it to EAGAIN...
    void *buffer[MAX_STACK_DEPTH];
    printf("errno == %d\n", errno);
    int n = backtrace(buffer, MAX_STACK_DEPTH);

    fprintf(stderr, "Stack trace (most recent call first):\n");
    for (int i = 0; i < n; i++) {
        Dl_info info;
        void *addr = buffer[i];

        if (!(dladdr(addr, &info) && info.dli_sname)) {
            fprintf(stderr, "  [%02d] %p -- no symbol info\n", i, addr);
            continue;
        }

        uintptr_t offset = (uintptr_t)addr - (uintptr_t)info.dli_saddr;
        char bin_pathname[MAX_BIN_NAME_LEN] = "";

        // Get the
        readlink("/proc/self/exe", bin_pathname, MAX_BIN_NAME_LEN);

        // Choose the correct object file for this frame
        const char *obj = info.dli_fname ? info.dli_fname : bin_pathname;

        char cmd[MAX_CMD_LEN];
        snprintf(cmd,
                 sizeof(cmd),
                 "addr2line -e %s -f -p %lx\n",
                 obj,
                 // Find the position in the memory where the function is
                 // located and subtract the base address from it to get a
                 // position in file. After that subtract 1 to get the line we
                 // actually want and not the line after it
                 (long)buffer[i] - (long)info.dli_fbase - 1);

        FILE *fp = popen(cmd, "r"); // Open the pipe and run the command
        if (!fp) {
            error(EIO, 0, "popen returned NULL in stack dump. Bailing now!\n");
        }

        char line[MAX_CMD_LEN];
        fprintf(stderr, "  [%02d] ", i);
        if (fgets(line, sizeof(line), fp) && !strstr(line, ":?"))
            fprintf(stderr, "%s", line);
        else
            fprintf(stderr, "%s + 0x%lx\n", info.dli_sname, offset);
        pclose(fp);
    }
    errno = temp_errno; // restore the old errno if nothing went wrong
    return n;
}

    // Tests and asserts
    #define TERM_GRAY       "30"
    #define TERM_RED        "31"
    #define TERM_GREEN      "32"
    #define TERM_YELLOW     "33"
    #define TERM_BLUE       "34"
    #define TERM_PURPLE     "35"
    #define TERM_CYAN       "36"
    #define TERM_WHITE      "37"

    #define TERM_NORMAL     "0"
    #define TERM_BOLD       "1"
    #define TERM_LIGHT      "2"
    #define TERM_UNDERLINED "4"
    #define TERM_BLINKING   "5"
    #define TERM_REVERSE    "7"

    #define TERM_TEXT_MOD(mod, color, text) \
        "\033[" mod ";" color "m" text "\033[0;37m"

    #define ASSERT(e)                                                          \
        ({                                                                     \
            bool result = e;                                                   \
            char *ans   = result                                               \
                              ? TERM_TEXT_MOD(TERM_NORMAL, TERM_GREEN, "PASS") \
                              : TERM_TEXT_MOD(TERM_NORMAL, TERM_RED, "FAIL");  \
            printf("%s:%d: assertion ’%s’ %s\n",                               \
                   __FILE__,                                                   \
                   __LINE__,                                                   \
                   ans,                                                        \
                   TERM_TEXT_MOD(TERM_LIGHT, TERM_WHITE, #e));                 \
            if (!result) {                                                     \
                print_stacktrace();                                            \
                abort();                                                       \
            }                                                                  \
        })

    #define TEST(func, expect, ...)                                          \
        ({                                                                   \
            typeof(expect) result = func;                                    \
                                                                             \
            char format[512];                                                \
            snprintf(format,                                                 \
                     512,                                                    \
                     "%s%s%s",                                               \
                     "%s:%d: assertion ’%s’ %s -> ",                         \
                     FORMAT(expect),                                         \
                     " == %s\n");                                            \
            char *ans = (result == expect)                                   \
                            ? TERM_TEXT_MOD(TERM_NORMAL, TERM_GREEN, "PASS") \
                            : TERM_TEXT_MOD(TERM_NORMAL, TERM_RED, "FAIL");  \
            printf(format,                                                   \
                   __FILE__,                                                 \
                   __LINE__,                                                 \
                   ans,                                                      \
                   TERM_TEXT_MOD(TERM_LIGHT, TERM_WHITE, #func),             \
                   result,                                                   \
                   #expect);                                                 \
            assert(result == expect);                                        \
        })
#endif // TRACINGTOOLS_H_
