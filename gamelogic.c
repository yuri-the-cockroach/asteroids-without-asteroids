#include "headers/gamelogic.h"

void KeyboardHandler(PlayerStruct *player) {

    float adjMoveSpeed = player->moveSpeed * GetFrameTime();
    float adjRotateSpeed = player->rotateSpeed * GetFrameTime();
    float rotateBy = 0;

    if (IsKeyDown('W')) OnAccelerate(player, adjMoveSpeed);
    if (IsKeyDown('S')) OnAccelerate(player, -adjMoveSpeed);
    if (IsKeyDown('D')) rotateBy += adjRotateSpeed;
    if (IsKeyDown('A')) rotateBy -= adjRotateSpeed;

    MutTransformPlayerMatrix(player, rotateBy);
}
