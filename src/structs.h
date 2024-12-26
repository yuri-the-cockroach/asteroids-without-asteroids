#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

// Sys stuff
#include <sys/time.h>

// Include
#include <err.h>
// StdThingings
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Local Headers

// -------------------- enums --------------------

enum loglevel {
    NOLOG     = 0,
    FATAL     = 1,
    ERROR     = 2,
    WARNING   = 3,
    BENCH     = 4,
    INFO      = 5,
    TEST_FAIL = 6,
    TEST_PASS = 7,
    FIXME     = 8,
    DEBUG     = 9,
    TRACE     = 10,
    ALL       = 11,
};

enum game_state {
    INIT      = 0,
    MAIN_MENU = 1,
    START_NEW = 2,
    GAME_OVER = 3,
    RUNNING   = 4,
    TESTING   = 5,
    PAUSE     = 6,
    EXIT      = -1,
    CLEANUP   = -2,
    NOOP      = -255
};

enum difficulty {
    GAME_JOURNALIST = 0,
    LOW             = 1,
    MEDIUM          = 2,
    HIGH            = 3,
    VERY_HIGH       = 4,
    INSANE          = 5,
    DOFH            = 10
};

enum menuOptionType { DUMMY = -1, SUBMENU = 1, FUNCTION = 2 };

static const char *loglvlToString[12] = { "NOLOG",
                                          "\033[1;31mFATAL\033[0;37m",
                                          "\033[0;31mERROR\033[0;37m",
                                          "\033[1;33mWARNING\033[0;37m",
                                          "\033[0;33mBENCH\033[0;37m",
                                          "\033[0;32mINFO\033[0;37m",
                                          "\033[1;31mTEST FAIL\033[0;37m",
                                          "\033[0;32mTEST PASS\033[0;37m",
                                          "\033[0;34mFIXME\033[0;37m",
                                          "\033[1;35mDEBUG\033[0;37m",
                                          "\033[0;35mTRACE\033[0;37m",
                                          "ALL" };

static const char *loglvlToStringNoColor[12] = {
    "NOLOG",     "FATAL",     "ERROR", "WARNING", "BENCH", "INFO",
    "TEST FAIL", "TEST PASS", "FIXME", "DEBUG",   "TRACE", "ALL"
};

enum request { IGNORE = 0, UPDATE = 1, SEPARATE = 2, CREATE = 3, DELETE = -1 };

enum type { NOTYPE = 0, ASTEROID = 1, PROJECTILE = 2, PLAYER = 3 };

// -------------------- typedefs --------------------

typedef struct obj_tracker_struct objTracker;
typedef struct object_wrap_struct objWrap;
typedef struct object_struct object;
typedef struct shape_struct shape;
typedef struct collider_struct collider;
typedef struct menu_function_wrap_struct menuFunctionWrap;
typedef struct menu_parent_struct menuParent;
typedef struct menu_option_struct menuOption;

// -------------------- CONSTANTS --------------------
// -------------------- Physics constants --------------------

static const float ELASTICITY_FACTOR =
    0.5; // How much energy object recives back on bouce

static const int WORLD_POS_MIN_X = 0;
static const int WORLD_POS_MAX_X = 10000;

static const int WORLD_POS_MIN_Y = 0;
static const int WORLD_POS_MAX_Y = 10000;

static const int SOFT_MAX_ASTEROIDS = 1000;
static const int MAX_OBJECT_COUNT   = 1024;

// player related
static const int RATE_OF_FIRE          = 5;
static const int PROJECTILE_SPEED      = 20;
static const float PROJECTILE_SIZE     = 0.1f;
static const int PLAYER_ROTATION_SPEED = 5;
static const int PLAYER_MOVE_SPEED     = 20;
static const int BASE_ROTATE           = 5;

static const enum loglevel DEFAULT_LOG_LEVEL = WARNING;

static const int MAX_MENU_STACK_SIZE = 32;

static const long ASTEROID_SPAWN_DELAY            = 10;
static const unsigned long ASTEROID_CORNERS_COUNT = 20;
static const float ASTEROID_HEIGHT_VARIATION      = 15;

// Externally defined dynamic global variables

// Debugging vars
#ifdef DEBUGGING
extern objWrap *lastDragged;
extern Vector2 SPEED_PREV;
extern bool DEBUG_PAUSE;
extern bool VISUAL_DEBUG;
extern bool VISUAL_DEBUG_SHOW_POINTS;
extern bool GDB_BREAK;
#endif // DEBUGGING

extern long N_CPU_THREADS;

// Logging vars
extern enum loglevel CURRENT_LOG_LEVEL_CONSOLE;
extern enum loglevel CURRENT_LOG_LEVEL_FILE;

extern enum difficulty CUR_DIFFICULTY;
extern char LOG_FILE_NAME[64];
extern FILE *LOG_FILE_PTR;

// Benchmarking
#ifdef BENCHMARKING
extern bool BENCHRUNNING;
extern long BENCH_COLLIDER_TIME;
extern FILE *BENCH_LOG_FILE_PTR;

static const char *restrict BENCH_LOG_FILE_NAME = "asteroids-benchlog.log";
static const int SAMPLES                        = 1024;
#endif // BENCHMARKING

// Game flow related
extern float GAME_TIME_PASSED;
extern float NEXT_ASTEROID_SPAWN;
extern float LAST_ASTEROID_SPAWN;
extern enum game_state NEXT_STATE;
extern enum game_state GAME_STATE;
extern long LAST_SHOT;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int FPS_TARGET;
extern bool CAMERA_FOLLOW;

static const Vector2 PLAYER_SHAPE_POINTS[] = {
    (Vector2){ 0,   -50 }, // Is always treated as the fronts-most point, used to
                         // get the heading
    (Vector2){ 50,  50  },
    (Vector2){ 0,   20  },
    (Vector2){ -50, 50  }
};

static const Vector2 ASTEROID_SHAPE_POINTS[] = {
    (Vector2){ -50, 50  },
    (Vector2){ 50,  50  },
    (Vector2){ 50,  -50 },
    (Vector2){ -50, -50 }
};

static const Vector2 PROJECTILE_SHAPE_POINTS[] = {
    (Vector2){ 0,   -50 },
    (Vector2){ -30, 40  },
    (Vector2){ 45,  -15 },
    (Vector2){ -45, -15 },
    (Vector2){ 30,  40  }
};

// -------------------- structs --------------------

struct mt_data_wrap {
    pthread_t *threads;
    struct mt_init_struct *initStructList;
};

struct mt_init_struct {
    pthread_mutex_t threadMutex;
    pthread_cond_t threadWakeupSignal;
    objTracker *tracker;
    unsigned int id;
    bool KeepRunning;
};

struct shape_struct {
    float sizeMult;
    unsigned int arrayLength;
    Vector2 *points;
    Vector2 *refPoints;
};

struct object_struct {
    float rotateSpeed;
    float heading;
    Vector2 position;
    Vector2 speed;
    shape shape;
};

struct collider_struct {
    bool isCollidable; // True if object's collision is enabled
    Rectangle collider;
    float mass;
    void (*ActionOnCollision)(objTracker *tracker, objWrap *first,
                              objWrap *second);
};

struct object_wrap_struct {
    enum type objectType;
    enum request request;   // What to do with this object on the next iteration
                            // will be called on it
    bool updatePosition;    // True if object's position needs to be updated
    bool draw;              // True if object needs to be drawn
    bool isRotatableByGame; // Constant rotation speed, if needed. Ignored if 0
    object *objPtr;         // Pointer to the actuall object that is governed

    collider collider;
    int livesLeft;
};

struct obj_tracker_struct {
    objWrap *playerPtr;
    Camera2D playerCamera;
    objWrap **objList;
    unsigned long objListLen;
    unsigned int playerScore;
};

struct menu_function_wrap_struct {
    void (*func)(void *args);
    void *payload;
};

struct menu_option_struct {
    const char *restrict name;
    enum menuOptionType type;
    const void *menuOptionData;
};

struct menu_parent_struct {
    const char *restrict name;
    int optionListLen;
    struct menu_option_struct *optionList;
};

// -------------------- end --------------------
#endif // STRUCTS_H_
