#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include "structs.h"
#include "objecthandler.h"
#include "gamelogic.h"
#include "render.h"
#include "structs.h"
#include "menulogic.h"

void *RenderJob(void* pointer);
int StateMachine(void);
void RunConfig(void);

#endif // STATEMACHINE_H_
