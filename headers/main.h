#ifndef MAIN_H_
#define MAIN_H_

#include <raylib.h>
#include <rcamera.h>
#include <raymath.h>
#include <signal.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Local Headers
#include "syslogic.h"
#include "objectlogic.h"
#include "gamelogic.h"

const Vector2 PLAYER_SHAPE_POINTS[] = {
(Vector2){   0, -100 }, // Is always treated as the fronts-most point, used to get the heading
(Vector2){  50,   50 },
(Vector2){   0,    0 },
(Vector2){ -50,   50 }
};

const Vector2 ASTEROID_SHAPE_POINTS[] = {
(Vector2){-50,   50  },
(Vector2){ 50,   50  },
(Vector2){50,  -50  },        // Is always treated as the fronts-most point, used to get the heading
(Vector2){ -50,  -50  }
};


#endif // MAIN_H_
