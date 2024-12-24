#include "structs.h"

#ifndef DEFAULTS
#define DEFAULTS

enum loglevel CURRENT_LOG_LEVEL_CONSOLE = DEFAULT_LOG_LEVEL;
enum loglevel CURRENT_LOG_LEVEL_FILE = DEFAULT_LOG_LEVEL;

int SCREEN_WIDTH = 1600;
int SCREEN_HEIGHT = 900;

int FPS_TARGET = 75;

long LAST_SHOT = 0;
bool CAMERA_FOLLOW = true;
float LAST_ASTEROID_SPAWN = 0;
float NEXT_ASTEROID_SPAWN = 0;
float GAME_TIME_PASSED = 0;
enum difficulty CUR_DIFFICULTY = 0;

#ifdef DEBUGGING
objWrap *lastDragged = NULL;
Vector2 SPEED_PREV = { 0, 0 };
bool GDB_BREAK = false;
bool DEBUG_PAUSE = false;
bool VISUAL_DEBUG_SHOW_POINTS = false;
bool VISUAL_DEBUG = false;
#endif // DEBUGGING

#ifdef BENCHMARKING
bool BENCHRUNNING = false;
long BENCH_COLLIDER_TIME = 0;
FILE *BENCH_LOG_FILE_PTR;
#endif // BENCHMARKING

char LOG_FILE_NAME[64];
FILE *LOG_FILE_PTR;

#endif
