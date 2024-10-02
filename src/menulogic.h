#ifndef MENULOGIC_H_
#define MENULOGIC_H_

#include "structs.h"

// Execute currently highlighted action
int SelectCurrent(struct menuParent *menu);

void TriggerGameOver(void);

// Sets game state to NEW_GAME, because there's no lambda functions in C
void TriggerNewGame(void);

// Sets game state to EXIT, because there's no lambda functions in C
void TriggerExitGame(void);

// Push an entry to menu stack
int MenuStackPush(struct menuStack *stack, struct menuParent menu);

// Pop an entry from menu stack
struct menuParent MenuStackPop(struct menuStack *stack);

// Empty the menu stack
int MenuStackClear(struct menuStack *stack);


struct menuParent SetupMainMenu(void);

struct menuParent SetupPauseMenu(void);

struct menuParent SetupGameOverMenu(void);


void TriggerMainMenu(void);
void TriggerRestart(void);

// Get the entry from the top of the stack without removing it from the stack
struct menuParent* MenuStackGet(struct menuStack *stack);

#endif // MENULOGIC_H_
