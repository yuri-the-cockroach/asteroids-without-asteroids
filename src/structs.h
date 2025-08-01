#ifndef STRUCTS_H_
#define STRUCTS_H_

#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"

// Sys stuff
#include <sys/time.h>

// Include
#include <err.h>
// StdThingings
#include <pthread.h>
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
    INFO      = 4,
    TEST_FAIL = 5,
    TEST_PASS = 6,
    FIXME     = 7,
    DEBUG     = 8,
    TRACE     = 9,
    ALL       = 10,
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

#define TERM_GRAY       "30"
#define TERM_RED        "31"
#define TERM_GREEN      "32"
#define TERM_YELLOW     "33"
#define TERM_BLUE       "34"
#define TERM_PURPLE     "35"
#define TERM_CYAN       "36"
#define TERM_WHITE      "37"

#define TERM_NORMAL     "0"
#define TERM_BOLD       "1"
#define TERM_LIGHT      "2"
#define TERM_UNDERLINED "4"
#define TERM_BLINKING   "5"
#define TERM_REVERSE    "7"

#define TERM_TEXT_MOD(mod, color, text) \
    "\033[" mod ";" color "m" text "\033[0;37m"

extern const char *loglvlToString[12];

// "\e[1;33mWARNING\e[0m",
// "\e[0;32mINFO\e[0m",
// "\e[1;31mTEST FAIL\e[0m",
// "\e[0;32mTEST PASS\e[0m",
// "\e[0;34mFIXME\e[0m",
// "\e[1;35mDEBUG\e[0m",
// "\e[0;35mTRACE\e[0m",
extern const char *loglvlToStringNoColor[12];

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

#define LOG_FILE_NAME_SIZE 64

extern const float WORLD_POS_MIN_X;
extern const float WORLD_POS_MAX_X;

extern const float WORLD_POS_MIN_Y;
extern const float WORLD_POS_MAX_Y;
extern const unsigned int ASTEROID_POINT_HIGHT;
extern const float MAX_ASTEROID_SIZE;

extern const unsigned int MAX_OBJECT_COUNT;
extern const unsigned int SOFT_MAX_ASTEROIDS;

// player related
extern const int RATE_OF_FIRE;
extern const float PROJECTILE_SPEED;
extern const float PROJECTILE_SIZE;
extern const int PLAYER_ROTATION_SPEED;
extern const int PLAYER_MOVE_SPEED;
extern const int BASE_ROTATE;
extern const float BOUNCE_BACK_FACTOR;

extern const enum loglevel DEFAULT_LOG_LEVEL;

extern const long ASTEROID_SPAWN_DELAY;
extern const unsigned long ASTEROID_CORNERS_COUNT;
extern const float ASTEROID_HEIGHT_VARIATION;

// Externally defined dynamic global variables

extern float LAST_FRAME_TIME;

// Debugging vars
#ifdef DEBUGGING
extern int COLLISION_COUNT;
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
extern char LOG_FILE_NAME[LOG_FILE_NAME_SIZE];
extern FILE *LOG_FILE_PTR;

// Benchmarking
#ifdef BENCHMARKING
extern bool BENCHRUNNING;
extern long BENCH_COLLIDER_TIME;
extern FILE *BENCH_LOG_FILE_PTR;

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

extern const int PLAYER_SHAPE_SIZE;
extern const Vector2 PLAYER_SHAPE_POINTS[];
extern const int PROJECTILE_SHAPE_SIZE;
extern const Vector2 PROJECTILE_SHAPE_POINTS[];

extern const float MAX_COLL_OFFSET;

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
    pthread_mutex_t mutex;
};

struct obj_tracker_struct {
    objWrap *playerPtr;
    Camera2D playerCamera;
    objWrap **objList;
    unsigned long objListLen;
    unsigned int playerScore;
    struct {
        Vector2 screenStart, screenEnd;
    } screenBorderWrap;
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
