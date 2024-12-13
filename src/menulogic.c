#include "autils.h"
#include "logger.h"
#include "menulogic.h"
#include "structs.h"

#pragma GCC diagnostic ignored "-Wvoid-pointer-to-enum-cast"

void TriggerGameOver(void *args) {
    UNUSED(args);
    GAME_STATE = GAME_OVER;
}

void SetDifficulty(void *args) {
    CUR_DIFFICULTY = (enum difficulty)args;
    TriggerNewGame(NULL);
}

void TriggerRestart(void *args) {
    UNUSED(args);
    GAME_STATE = CLEANUP;
    NEXT_STATE = START_NEW;
}

void TriggerNewGame(void *args) {
    UNUSED(args);
    GAME_STATE = START_NEW;
}

void TriggerMainMenu(void *args) {
    UNUSED(args);
    GAME_STATE = CLEANUP;
    NEXT_STATE = MAIN_MENU;
}

void TriggerTesting(void *args) {
    UNUSED(args);
    GAME_STATE = TESTING;
}

void TriggerExitGame(void *args) {
    UNUSED(args);
    GAME_STATE = CLEANUP;
    NEXT_STATE = EXIT;
}

const menuParent *SelectCurrent(const menuParent *menu, const int menuHighlighted) {
    menuOption *selected = &menu->optionList[menuHighlighted];
    switch (selected->type) {
        case FUNCTION: {
            const menuFunctionWrap *funcWrap = (const menuFunctionWrap*)selected->menuOptionData;
            funcWrap->func(funcWrap->payload);
            break;
        }
        case SUBMENU: {
            return (const menuParent*)selected->menuOptionData;
        }

        case DUMMY: {
            LOG(INFO, "%s", "Got a dummy menu object selected, dono what to do next...");
            break;
        }
    }
    return menu;
}
