#include "gamelogic.h"

void KeyboardHandler(ObjectStruct *object) {

    float adjMoveSpeed = object->moveSpeed * GetFrameTime();
    float adjRotateSpeed = object->rotateSpeed * GetFrameTime();

    if (IsKeyDown('W')) OnObjectAccelerate(object, adjMoveSpeed);
    if (IsKeyDown('S')) OnObjectAccelerate(object, -adjMoveSpeed);
    if (IsKeyDown('D')) object->shape.TransformShapeMatrix(&object->shape, object->heading += adjRotateSpeed);
    if (IsKeyDown('A')) object->shape.TransformShapeMatrix(&object->shape, object->heading -= adjRotateSpeed);
}
