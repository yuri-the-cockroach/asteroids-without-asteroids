/* #include "objectHandler.h" */
#include <stdlib.h>

typedef struct tracker ObjTracker;
typedef struct object Object;
enum action {
    IGNORE = 1,
    UPDATE = 0,
    DELETE = -1
};

enum type {
    GENERIC = 0,
    PLAYER = 1,
    ASTEROID = 2,
    RESERVED = -1
};

struct object {
    enum action request;
    enum type objType;
    void *objPtr;
    void (*UpdateObj)(Object *self);
    void (*DeleteObj)(Object *self);
};

struct tracker {
    unsigned int objCount;
    Object **objArray;
};

void RunActionList(ObjTracker *self) {
    for (int i = 0; i < self->objCount; i++) {
        Object *current = self->objArray[i];

        if (current == NULL) {
            continue;
        }

        switch (current->request) {
            case IGNORE:
                continue; // next for cycle

            case DELETE:
                current->DelPhysObj(current);
                self->objArray[self->physObjCount] = 0; // TODO: this case needs to be handled properly, otherwise there's a risk to endup with an array of 0s
                break;

            case UPDATE:
                current->UpdateObj(current);
                break;
        }
    }
}

void UpdateObj(Object *self);
void DeleteObj(Object *self);
