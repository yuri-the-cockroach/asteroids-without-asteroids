
#include <raylib.h>
#include <rcamera.h>
#include <raymath.h>
#include <signal.h>
#include <err.h>
#include <stdbool.h>
#include <errno.h>

// Local Headers
#include "headers/syslogic.h"
#include "headers/playerlogic.h"
#include "headers/gamelogic.h"
#include "headers/drawing.h"



// Retuns prefilled PlayerStruct with default data

int main() {
    errno = 0;
    signal(SIGINT, SigIntHandler);

    const int screenWidth = 1280;
    const int screenHight = 1080;

    int fpsTarget = 120;


    InitWindow(screenWidth, screenHight, "Something something artillery");

    SetWindowPosition(2560 + 900 - screenWidth, 0);
    SetWindowMinSize(10, 10);
    SetWindowMaxSize(8192, 8192);

    SetTargetFPS(fpsTarget);


    /* --------------------- Player Init -------------------- */

    PlayerStruct player = InitPlayer();
    player.position = (Vector2){ 200, 200 };

    while (!WindowShouldClose()) {
         // Adjust moving speed for current frametime, so you don't get faster as fps goes up
        KeyboardHandler(&player); // All the keybindings and movement

        UpdatePlayerPos(&player);

        if (IsKeyPressed('9')) SetTargetFPS(fpsTarget = 75);
        if (IsKeyPressed('0')) SetTargetFPS(fpsTarget = 0);
        if (IsKeyPressed('=')) SetTargetFPS(fpsTarget += 10);
        if (IsKeyPressed('-')) SetTargetFPS(fpsTarget -= 10);
        BeginDrawing();


            ClearBackground(BLACK);

            /* DrawRectangle(player.position.x, player.position.y, 100, 100, RED); */
            DrawPlayer(&player);
            DrawFPS(400, 0);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
