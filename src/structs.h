#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>


// Sys stuff
#include <sys/time.h>
#include <signal.h>

// Include
#include <err.h>
#include <errno.h>

// StdThingings
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Local Headers

/* -------------------- enums --------------------  */

enum loglevel {
    NOLOG = 0,
    FATAL = 1,
    ERROR = 2,
    WARNING = 3,
    INFO = 4,
    FIXME = 5,
    DEBUG = 6,
    TRACE = 7,
    ALL = 8
};

enum game_state {
    INIT = 0,
    MAIN_MENU = 1,
    START_NEW = 2,
    GAME_OVER = 3,
    RUNNING = 4,
    PAUSE = 5,
    EXIT = -1,
};

static const char *loglvlToString[9] = {
    "NOLOG",
    "\033[1;31mFATAL\033[0;37m",
    "\033[0;31mERROR\033[0;37m",
    "\033[1;33mWARNING\033[0;37m",
    "\033[0;32mINFO\033[0;37m",
    "\033[0;34mFIXME\033[0;37m",
    "\033[1;35mDEBUG\033[0;37m",
    "\033[0;35mTRACE\033[0;37m",
    "ALL"
};

enum request { UPDATE = 1, IGNORE = 0,  DELETE = -1 };
enum type { NOTYPE = 0, ASTEROID = 1, PLAYER = 2, PROJECTILE = 3 };

/* -------------------- typedefs --------------------  */

typedef struct tracker ObjectTracker;
typedef struct ObjectWrap ObjectWrap;
typedef struct ObjectStruct ObjectStruct;
typedef struct ShapeStruct ShapeStruct;
typedef struct Collider Collider;

/* -------------------- constants --------------------  */


/* -------------------- Physics constants --------------------  */

static const float ELASTICITY_FACTOR = 0.5; // How much energy object recives back on bouce

static const int WORLD_POS_MIN_X = -5000;
static const int WORLD_POS_MAX_X = 5000;

static const int WORLD_POS_MIN_Y = -5000;
static const int WORLD_POS_MAX_Y = 5000;

static const int RATE_OF_FIRE = 5;
static const int PROJECTILE_SPEED = 20;
static const float PROJECTILE_SIZE = 0.1f;
static const int MAX_OBJECT_COUNT = 1024;
static const int PLAYER_ROTATION_SPEED = 5;
static const int PLAYER_MOVE_SPEED = 5;

static const int BASE_ACCELL = 5;
static const int BASE_ROTATE = 5;

static const int BORDER_OFFSET = 40;
static const enum loglevel DEFAULT_LOG_LEVEL = WARNING;

static const int MAX_MENU_STACK_SIZE = 32;
static const unsigned long ASTEROID_CORNERS_COUNT = 20;
static const float ASTEROID_HEIGHT_VARIATION = 10;


/* Externally defined dynamic global variables */

/* Debugging vars */
extern bool DEBUGGING;
extern bool DEBUG_PAUSE;
extern bool VISUAL_DEBUG;
extern enum loglevel CURRENT_LOG_LEVEL;
extern bool GDB_BREAK;

/* Game flow related */
extern enum game_state GAME_STATE;
extern long lastShot;
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
    (Vector2){ 50,  -50 }, // Is always treated as the fronts-most point, used to
                          // get the heading
    (Vector2){ -50, -50 }
};

static const Vector2 PROJECTILE_SHAPE_POINTS[] = {
    (Vector2){ 0,   -50 },
    (Vector2){ -30, 40  },
    (Vector2){ 45,  -15 },
    (Vector2){ -45, -15 },
    (Vector2){ 30,  40  }
};

/* -------------------- structs --------------------  */

struct ShapeStruct {
    float sizeMult;
    unsigned int arrayLength;
    Vector2 *points;
    Vector2 *refPoints;
    /* Function references */
};

struct ObjectStruct {
    float rotateSpeed;
    float heading;
    Vector2 position;
    Vector2 speed;
    ShapeStruct shape;
};


struct Collider {
    bool isCollidable;          // True if object's collision is enabled
    Rectangle collider;
    float mass;

    void (*ActionOnCollision)(ObjectWrap *first, ObjectWrap *second);
    int collidedListLen;
    ObjectWrap **collidedList;

};

struct ObjectWrap {
    enum type objectType;
    enum request request;   // What to do with this object on the next iteration
                            // will be called on it
    bool updatePosition;    // True if object's position needs to be updated
    bool draw;              // True if object needs to be drawn
    bool isRotatableByGame; // Constant rotation speed, if needed. Ignored if 0
    ObjectStruct *objPtr;   // Pointer to the actuall object that is governed

    Collider collider;
};

struct tracker {
    ObjectWrap *playerPtr;
    Camera2D playerCamera;
    ObjectWrap **objList;
    unsigned long objListLen;
};

struct menuOption {
    char *name;
    void (*MenuAction) (void);
};

struct menuParent {
    int selected;
    int optionListLen;
    struct menuOption *optionList;
};

struct menuStack {
    int stackSize;
    struct menuParent *optionList;
};

/* -------------------- end --------------------  */
#endif // STRUCTS_H_
