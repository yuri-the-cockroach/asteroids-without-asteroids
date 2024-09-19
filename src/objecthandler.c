#include "objecthandler.h"
#include "objectlogic.h"
#include <raylib.h>

// Returns a list of objects that need to be drawn
void RunActionList(ObjectTracker *self) {

    for (unsigned long i = 0; i < self->objListLen; i++) {
        ObjectWrap *current = self->objList[i];

        if (current == NULL) {
            continue;
        }

        switch (current->request) {
        case IGNORE:
            continue; // next for cycle

        case DELETE:
            current->WrapDestructor(current);
            self->objList[self->objListLen] =
                0; // TODO: this case needs to be handled properly, otherwise
            // there's a risk to endup with an array of 0s
            break;

        case UPDATE:
            current->UpdateObj(current);
            break;
        }
    }
}

void UpdateObj(ObjectWrap *self) {
    if (self->player)
        KeyboardHandler(self->objPtr);
    if (self->updatePosition)
        UpdateObjectPos(self->objPtr);

    if (self->cRotSpeed) {
        self->objPtr->shape.TransformShapeMatrix(&self->objPtr->shape, self->objPtr->heading += (self->cRotSpeed * GetFrameTime()));
    }
    if (self->draw)
        AddToDrawList(self->parent, self->objPtr);
}

void AddToDrawList(ObjectTracker *self, ObjectStruct *obj) {
    if (self->drawListLen >= MAX_OBJECT_COUNT) {
        fprintf(stderr,
                "Error in AddToDrawList, too many objects in "
                "self->drawList\nself->drawListLen is at %ld",
                self->drawListLen);
        exit(ECHRNG);
    }
    self->drawList[self->drawListLen] = obj;
    self->drawListLen++;
}

void DrawAllFromDrawList(ObjectTracker *self) {
    if (self->drawListLen == 0) {
        printf("Warning: Draw list is empty, not drawing anything\n");
        return;
    }

    for (unsigned int i = 0; i < self->drawListLen; i++) {
        DrawObject(self->drawList[i]);
        self->drawList[i] = NULL;
    }
    self->drawListLen = 0;
}

ObjectTracker InitTracker() {
    return (ObjectTracker){
        false,
        (ObjectWrap **)calloc(MAX_OBJECT_COUNT, sizeof(ObjectWrap *)),
        0,
        (ObjectStruct **)calloc(MAX_OBJECT_COUNT, sizeof(ObjectStruct *)),
        0,
        *DeleteTrackedObject
    };
}

ObjectWrap InitWrap() {
    return (ObjectWrap){ NULL,  0, IGNORE, false,      false,         false,
                         false, 0, NULL,   *UpdateObj, *DeleteObjWrap };
}

void AddWrapToList(ObjectTracker *self, ObjectWrap *wrap) {
    if (self->objListLen >= MAX_OBJECT_COUNT) {
        fprintf(stderr,
                "Error in AddWrapToList, too many objects in "
                "self->objList\nself->objListLen is at %ld",
                self->objListLen);
        exit(ECHRNG);
    }
    wrap->parent = self;
    wrap->index = self->objListLen;
    self->objList[self->objListLen] = wrap;
    self->objListLen++;
}

// Pass the default speed, default rotation speed, default position
void CreatePlayer(ObjectTracker *self, Vector2 initPosition,
                  unsigned int accelSpeed, unsigned int rotSpeed) {
    ObjectWrap *player = malloc(sizeof(ObjectWrap));
    player[0] = InitWrap();

    player->request = UPDATE;
    player->player = true;
    player->updatePosition = true;
    player->draw = true;
    player->collider = true;

    player->objPtr = malloc(sizeof(ObjectStruct));
    player->objPtr[0] =
        InitObject(InitShape(PLAYER_SHAPE_POINTS,
                             sizeof(PLAYER_SHAPE_POINTS) / sizeof(Vector2),
                             0.5),
                   initPosition,
                   (Vector2){ 0, 0 },
                   accelSpeed,
                   rotSpeed);
    AddWrapToList(self, player);
}

void CreateAsteroid(ObjectTracker *self, Vector2 initPosition,
                    Vector2 initSpeed, int constRotationSpeed) {
    ObjectWrap *asteroid = malloc(sizeof(ObjectWrap));
    asteroid[0] = InitWrap();
    asteroid->request = UPDATE;
    asteroid->updatePosition = true;
    asteroid->draw = true;
    asteroid->collider = true;
    asteroid->cRotSpeed = constRotationSpeed;
    asteroid->objPtr = malloc(sizeof(ObjectStruct));
    asteroid->objPtr[0] =
        InitObject(InitShape(ASTEROID_SHAPE_POINTS,
                             sizeof(ASTEROID_SHAPE_POINTS) / sizeof(Vector2),
                             0.5),
                   initPosition,
                   initSpeed,
                   0,
                   0);
    AddWrapToList(self, asteroid);
}

void DeleteObjWrap(ObjectWrap *self) {
    DeleteObjectStruct(self->objPtr);
    free(self);
}

void DeleteTrackedObject(ObjectTracker *self, ObjectWrap *wrap) {
    unsigned long index = wrap->index;
    wrap->WrapDestructor(wrap);

    self->objListLen--;
    if (self->objListLen == index) {
        self->objList[index] = 0;
        return;
    }

    self->objList[index] = self->objList[self->objListLen];
    self->objList[self->objListLen] = 0;
}
