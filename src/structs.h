#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <raylib.h>

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

static const char *loglvlToString[9] = {
    "NOLOG",
    "\033[1;31mFATAL\033[0m",
    "\033[0;31mERROR\033[0m",
    "\033[1;33mWARNING\033[0m",
    "\033[0;32mINFO\033[0m",
    "\033[0;34mFIXME\033[0m",
    "\033[1;35mDEBUG\033[0m",
    "\033[0;35mTRACE\033[0m",
    "ALL"
};

enum request { UPDATE = 1, IGNORE = 0,  DELETE = -1 };
enum type { NOTYPE = 0, ASTEROID = 1, PLAYER = 2, PROJECTILE = 3 };

/* -------------------- typedefs --------------------  */

typedef struct tracker ObjectTracker;
typedef struct ObjectWrap ObjectWrap;
typedef struct ObjectStruct ObjectStruct;
typedef struct ShapeStruct ShapeStruct;

/* -------------------- constants --------------------  */

static const int RATE_OF_FIRE = 20;
static const int PROJECTILE_SPEED = 20;
static const double PROJECTILE_SIZE = 0.1;
static const int MAX_OBJECT_COUNT = 1024;
static const int PLAYER_ROTATION_SPEED = 5;
static const int PLAYER_MOVE_SPEED = 5;

static const int BASE_ACCELL = 5;
static const int BASE_ROTATE = 5;

static const int BORDER_OFFSET = 40;
static const enum loglevel DEFAULT_LOG_LEVEL = WARNING;

extern enum loglevel CURRENT_LOG_LEVEL;
extern long lastShot;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int FPS_TARGET;

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
    Rectangle collider;
    float friction;
    float mass;
};

struct ObjectWrap {
    enum type objectType;
    enum request request;   // What to do with this object on the next iteration
                            // will be called on it
    bool updatePosition;    // True if object's position needs to be updated
    bool draw;              // True if object needs to be drawn
    bool collider;          // True if object's collision is enabled
    bool isRotatableByGame; // Constant rotation speed, if needed. Ignored if 0
    ObjectStruct *objPtr;   // Pointer to the actuall object that is governed

    void (*ActionOnCollision)(ObjectWrap *first, ObjectWrap *second);
};

struct tracker {
    bool hasPlayer;
    ObjectWrap **objList;
    unsigned long objListLen;
    ObjectStruct **drawList; // List of objects that need to be drawn
    unsigned long drawListLen;
};

void InitGlobals(void);

/* -------------------- end --------------------  */
#endif // STRUCTS_H_
