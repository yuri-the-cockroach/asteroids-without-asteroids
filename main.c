#include "src/render.h"
#include "src/structs.h"
#include "src/logger.h"
#include "src/asteroidsutils.h"
#include "src/syslogic.h"
#include "src/objecthandler.h"
#include "src/statemachine.h"

enum loglevel CURRENT_LOG_LEVEL = DEFAULT_LOG_LEVEL;

int SCREEN_WIDTH = 1600;
int SCREEN_HEIGHT = 900;

bool VISUAL_DEBUG = false;
bool DEBUGGING = false;
bool DEBUG_PAUSE = false;

int FPS_TARGET = 75;

long lastShot = 0;
bool CAMERA_FOLLOW = true;
bool GDB_BREAK = false;


int main(int argc, char **argv) {
    errno = 0;


    // Handle the startup arguments
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--loglevel"))
            {
                i++;

                int newLogLevel = ClampInt(atoi(argv[i]), 0, 8);
                if (newLogLevel) {
                    CURRENT_LOG_LEVEL = (enum loglevel)newLogLevel;
                    continue;
                }

                if (strcmp(argv[i], "0")) {
                    CURRENT_LOG_LEVEL = 0;
                    continue;
                }
            }

            if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--debugging")) {
                DEBUGGING = true;
                LOG(DEBUG, "%s", "Debugging is enabled");
                continue;
            }

            if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--visual")) {
                VISUAL_DEBUG = true;
                LOG(DEBUG, "%s", "Debugging is enabled");
                continue;
            }
        }
    }



    // Handle the interseption of signals
    signal(SIGINT, SigIntHandler);
    signal(SIGTERM, SigIntHandler);

    StateMachine();
    return 0;

}
