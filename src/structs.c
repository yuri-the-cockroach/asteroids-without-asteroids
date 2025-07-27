#ifndef DEFAULTS_H_
#define DEFAULTS_H_
#include "structs.h"

const enum loglevel DEFAULT_LOG_LEVEL   = WARNING;
enum loglevel CURRENT_LOG_LEVEL_CONSOLE = DEFAULT_LOG_LEVEL;
enum loglevel CURRENT_LOG_LEVEL_FILE    = DEFAULT_LOG_LEVEL;

int SCREEN_WIDTH  = 1600;
int SCREEN_HEIGHT = 900;

int FPS_TARGET = 75;

long LAST_SHOT                 = 0;
bool CAMERA_FOLLOW             = true;
float LAST_ASTEROID_SPAWN      = 0;
float NEXT_ASTEROID_SPAWN      = 0;
float GAME_TIME_PASSED         = 0;
enum difficulty CUR_DIFFICULTY = 0;

#ifdef DEBUGGING
objWrap *lastDragged          = NULL;
Vector2 SPEED_PREV            = { 0, 0 };
bool GDB_BREAK                = false;
bool DEBUG_PAUSE              = false;
bool VISUAL_DEBUG_SHOW_POINTS = false;
bool VISUAL_DEBUG             = false;
#endif // DEBUGGING

#ifdef BENCHMARKING
bool BENCHRUNNING        = false;
long BENCH_COLLIDER_TIME = 0;
FILE *BENCH_LOG_FILE_PTR;
#endif // BENCHMARKING

float LAST_FRAME_TIME = 0;

char LOG_FILE_NAME[64];
FILE *LOG_FILE_PTR;

long N_CPU_THREADS = 1;

const float MAX_ASTEROID_SIZE = 4;

#ifdef BENCHMARKING
const float WORLD_POS_MIN_X = 0.f;
const float WORLD_POS_MAX_X = 100000.f;
const float WORLD_POS_MIN_Y = 0.f;
const float WORLD_POS_MAX_Y = 100000.f;

const unsigned int MAX_OBJECT_COUNT   = 32768;
const unsigned int SOFT_MAX_ASTEROIDS = MAX_OBJECT_COUNT;

#else // NOT BENCHMARKING
const float WORLD_POS_MIN_X = 0;
const float WORLD_POS_MAX_X = 20000;
const float WORLD_POS_MIN_Y = 0;
const float WORLD_POS_MAX_Y = 20000;

const unsigned int SOFT_MAX_ASTEROIDS = 2000u;
const unsigned int MAX_OBJECT_COUNT   = 2048u;
#endif

const char *loglvlToString[12] = {
    "NOLOG",
    TERM_TEXT_MOD(TERM_REVERSE, TERM_RED, "FATAL"),
    TERM_TEXT_MOD(TERM_NORMAL, TERM_RED, "ERROR"),
    TERM_TEXT_MOD(TERM_NORMAL, TERM_YELLOW, "WARNING"),
    TERM_TEXT_MOD(TERM_NORMAL, TERM_GREEN, "INFO"),
    TERM_TEXT_MOD(TERM_BOLD, TERM_RED, "TEST FAIL"),
    TERM_TEXT_MOD(TERM_NORMAL, TERM_GREEN, "TEST PASS"),
    TERM_TEXT_MOD(TERM_LIGHT, TERM_GREEN, "FIXME"),
    TERM_TEXT_MOD(TERM_LIGHT, TERM_GREEN, "DEBUG"),
    TERM_TEXT_MOD(TERM_LIGHT, TERM_GREEN, "TRACE"),
    "ALL"
};
const char *loglvlToStringNoColor[12] = { "NOLOG",     "FATAL", "ERROR",
                                          "WARNING",   "INFO",  "TEST FAIL",
                                          "TEST PASS", "FIXME", "DEBUG",
                                          "TRACE",     "ALL" };

// player related
const int RATE_OF_FIRE          = 5;
const float PROJECTILE_SPEED    = 20.f;
const float PROJECTILE_SIZE     = 0.1f;
const int PLAYER_ROTATION_SPEED = 5;
const int PLAYER_MOVE_SPEED     = 20;
const int BASE_ROTATE           = 5;
const float BOUNCE_BACK_FACTOR  = 1;

// How far the point is from the center of the asteroid as a base
const unsigned int ASTEROID_POINT_HIGHT    = 50;
const long ASTEROID_SPAWN_DELAY            = 10;
const unsigned long ASTEROID_CORNERS_COUNT = 20;
const float ASTEROID_HEIGHT_VARIATION      = 15;

const Vector2 PLAYER_SHAPE_POINTS[] = {
    (Vector2){ 0,   -50 }, // Is always treated as the fronts-most point, used to
                         // get the heading
    (Vector2){ 50,  50  },
    (Vector2){ 0,   20  },
    (Vector2){ -50, 50  }
};

const int PLAYER_SHAPE_SIZE = sizeof(PLAYER_SHAPE_POINTS) / sizeof(Vector2);

const float MAX_COLL_OFFSET = MAX_ASTEROID_SIZE * -50;

const Vector2 PROJECTILE_SHAPE_POINTS[] = {
    (Vector2){ 0,   -50 },
    (Vector2){ -30, 40  },
    (Vector2){ 45,  -15 },
    (Vector2){ -45, -15 },
    (Vector2){ 30,  40  }
};
const int PROJECTILE_SHAPE_SIZE =
    sizeof(PROJECTILE_SHAPE_POINTS) / sizeof(Vector2);

#endif // DEFAULTS_H_
