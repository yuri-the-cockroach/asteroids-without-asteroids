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

struct menuParent* MenuStackGet(struct menuStack *stack) {
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

int MenuStackClear(struct menuStack *stack) {
    for ( int i = stack->stackSize - 1; i != 0; i-- ) {
        MenuStackPop(stack);
    }
    return 0;
}
struct menuParent SetupGameOverMenu(void) {
    const int numberOfItems = 3;
    struct menuOption *optionList = calloc(numberOfItems, sizeof(struct menuOption));
    optionList[0] = (struct menuOption){ "RESTART", *TriggerRestart };
    optionList[1] = (struct menuOption){ "MAIN MENU", *TriggerMainMenu };
    optionList[2] = (struct menuOption){ "EXIT", *TriggerExitGame };

    return (struct menuParent){ 0, numberOfItems, optionList};
}

struct menuParent SetupPauseMenu(void){
    const int numberOfItems = 3;
    struct menuOption *optionList = calloc(numberOfItems, sizeof(struct menuOption));
    optionList[0] = (struct menuOption){ "RESTART", *TriggerRestart };
    optionList[1] = (struct menuOption){ "MAIN MENU", *TriggerMainMenu };
    optionList[2] = (struct menuOption){ "EXIT", *TriggerExitGame };

    return (struct menuParent){ 0, numberOfItems, optionList};
}

struct menuParent SetupMainMenu(void) {
    const int numberOfItems = 2;
    struct menuOption *optionList = calloc(numberOfItems, sizeof(struct menuOption));
    optionList[0] = (struct menuOption){ "START GAME", *TriggerNewGame };
    optionList[1] = (struct menuOption){ "EXIT", *TriggerExitGame };

    return (struct menuParent){ 0, numberOfItems, optionList};
}
