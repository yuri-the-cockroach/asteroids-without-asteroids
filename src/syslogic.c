#include "syslogic.h"
#include "structs.h"

void SigIntHandler(int sig) {
    err(sig, "Got a signal interupt from console. Exiting now\n");
}

void SigSegvHandel(int sig) {
    char sym[256];
    unw_context_t context;
    unw_cursor_t cursor;
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);
    struct unw_proc_info procInfo;

    while (unw_step(&cursor) > 0) {
        unw_word_t offset, pc;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if (pc == 0) {
            break;
        }
        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
            unw_get_proc_info(&cursor,&procInfo);
            printf("%s+0x%lx\n", sym, offset);
        }
    }
    err(sig, "Segmentation fault. Exiting now\n");
}

