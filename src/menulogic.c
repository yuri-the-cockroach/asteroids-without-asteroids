#include "logger.h"
#include "menulogic.h"
#include "structs.h"

void TriggerGameOver(void) {
    GAME_STATE = GAME_OVER;
}

void TriggerRestart(void) {
    GAME_STATE = CLEANUP;
    NEXT_STATE = START_NEW;
}

void TriggerNewGame(void) {
    GAME_STATE = START_NEW;
}

void TriggerMainMenu(void) {
    GAME_STATE = CLEANUP;
    NEXT_STATE = MAIN_MENU;
}

void TriggerExitGame(void) {
    GAME_STATE = CLEANUP;
    NEXT_STATE = EXIT;
}

int SelectCurrent(struct menuParent *menu) {
    menu->optionList[menu->selected].MenuAction();
    return 0;
}

int MenuStackPush(struct menuStack *stack, struct menuParent menu) {
    if ( stack->stackSize >= MAX_MENU_STACK_SIZE ) {
        LOG(ERROR, "%s", "Menu stack size at maximum capacity");
        return -1;
    }
    stack->optionList[stack->stackSize] = menu;
    stack->stackSize++;
    return 0;
}

struct menuParent *MenuStackGet(struct menuStack *stack) {
    if ( stack->stackSize == 0 ) {
        LOG(WARNING, "%s", "Menu stack is empty. Returning empty struct");
        return NULL;
    }

    return &stack->optionList[stack->stackSize - 1];
}

struct menuParent MenuStackPop(struct menuStack *stack) {
    if ( stack->stackSize == 0 ) {
        LOG(WARNING, "%s", "Menu stack is empty. Returning empty struct");
        return (struct menuParent){ 0 };
    }
    stack->stackSize--;
    struct menuParent toReturn = stack->optionList[stack->stackSize];

    stack->optionList[stack->stackSize] = (struct menuParent){ 0 };
    return toReturn;
}
