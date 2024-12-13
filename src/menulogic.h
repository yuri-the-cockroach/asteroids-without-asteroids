#ifndef MENULOGIC_H_
#define MENULOGIC_H_

#include "structs.h"

// Execute currently highlighted action
const menuParent *SelectCurrent(const menuParent *menu, const int menuHighlighted);

void SetDifficulty(void *args);

void TriggerGameOver(void *args);

// Sets game state to NEW_GAME, because there's no lambda functions in C
void TriggerNewGame(void *args);

// Sets game state to EXIT, because there's no lambda functions in C
void TriggerExitGame(void *args);

void TriggerMainMenu(void *args);

void TriggerRestart(void *args);

// Get the entry from the top of the stack without removing it from the stack


// Const lists to be later included into structs

static struct menu_option_struct constDifficultyMenuList[] = {
(menuOption){"GAME_JOURNALIST", FUNCTION, &(menuFunctionWrap){*SetDifficulty, (void *)GAME_JOURNALIST}},
(menuOption){"LOW", FUNCTION, &(menuFunctionWrap){*SetDifficulty, (void *)LOW}},
(menuOption){"MEDIUM", FUNCTION, &(menuFunctionWrap){*SetDifficulty, (void *)MEDIUM}},
(menuOption){"HIGH", FUNCTION, &(menuFunctionWrap){*SetDifficulty, (void *)HIGH}},
(menuOption){"VERY_HIGH", FUNCTION, &(menuFunctionWrap){*SetDifficulty, (void *)VERY_HIGH}},
(menuOption){"INSANE", FUNCTION, &(menuFunctionWrap){*SetDifficulty, (void *)INSANE}},
(menuOption){"DOFH", FUNCTION, &(menuFunctionWrap){*SetDifficulty, (void *)DOFH}}
};


static struct menu_option_struct constPauseMenuList[] = {
    (menuOption){ "RESTART", FUNCTION, &(menuFunctionWrap){*TriggerRestart, NULL} },
    (menuOption){ "MAIN MENU", FUNCTION, &(menuFunctionWrap){*TriggerMainMenu, NULL} },
    (menuOption){ "EXIT", FUNCTION, &(menuFunctionWrap){*TriggerExitGame, NULL} }
};

// Actuall menus
static const menuParent refPauseMenu = (menuParent){
    .name          = "PAUSE",
    .optionListLen = sizeof(constPauseMenuList) / sizeof(menuOption),
    .optionList    = constPauseMenuList
};

static const menuParent refDifficultyMenu = (menuParent){
    .name          = "DIFFICULTY",
    .optionListLen = sizeof(constDifficultyMenuList) / sizeof(menuOption),
    .optionList    = constDifficultyMenuList
};


static struct menu_option_struct constMainMenuList[] = {
    (menuOption){ "START GAME", SUBMENU,  (const void*)&refDifficultyMenu},
    (menuOption){ "TESTING", FUNCTION,  &(menuFunctionWrap){*TriggerTesting, NULL} },
    (menuOption){ "EXIT", FUNCTION, &(menuFunctionWrap){*TriggerExitGame, NULL} }
};

static const  menuParent refMainMenu = (menuParent){
    .name          = "ASTEROIDS WITHOUT ASTEROIDS",
    .optionListLen = sizeof(constMainMenuList) / sizeof(menuOption),
    .optionList    = constMainMenuList
};
#endif // MENULOGIC_H_
