#include "AppTypes.h"
#include "Config.h"
#include "SortContext.h"
#include "SortView.h"
#include "raylib.h"
#include <cstdlib>
#include <ctime>

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    SortContext sim;
    sim.Scramble();
    sim.algo = SortAlgorithm::Bubble;
    sim.BeginSortRun(sim.algo);

    Scene scene = Scene::Sorting;

    InitWindow(Config::ScreenWidth, Config::ScreenHeight, "Sorting Visualizer by David Johansson");
    SetTargetFPS(60);

    float leftover_dt = 0.0f;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_TAB)) {
            scene = (scene == Scene::Menu) ? Scene::Sorting : Scene::Menu;
        }
        if (IsKeyPressed(KEY_ONE)) {
            sim.algo = SortAlgorithm::Bubble;
            sim.Scramble();
        }
        if (IsKeyPressed(KEY_TWO)) {
            sim.algo = SortAlgorithm::Insertion;
            sim.Scramble();
        }
        if (IsKeyPressed(KEY_THREE)) {
            sim.algo = SortAlgorithm::Heap;
            sim.Scramble();
        }
        if (IsKeyPressed(KEY_R)) {
            sim.Scramble();
        }
        if (IsKeyPressed(KEY_SPACE)) {
            sim.paused = !sim.paused;
        }

        if (scene == Scene::Sorting && !sim.paused && !sim.done) {
            leftover_dt += GetFrameTime();
            while (leftover_dt >= Config::STEP_DELAY) {
                leftover_dt -= Config::STEP_DELAY;
                sim.AdvanceOne();
                if (sim.done) {
                    break;
                }
            }
        } else {
            leftover_dt = 0.0f;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        RedrawBarStrip(sim);
        DrawHelpOverlay(sim, scene);
        DrawFPS(Config::ScreenWidth - 90, 12);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
