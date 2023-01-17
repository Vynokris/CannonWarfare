#include "App.h"
#include <raylib.h>
#include <rlImGui.h>
#include <imgui.h>
#include <chrono>
#include <thread>
#ifdef PLATFORM_WEB
    #include <emscripten/emscripten.h>
#endif
using namespace std::chrono;

// Web update function.
void UpdateAndDrawFrame()
{
    static App app({ 1728, 972 }, 60);

    // Main loop.
    app.Update(0);
    app.Draw();
}


int main(void)
{
    #ifdef PLATFORM_WEB
        emscripten_set_main_loop(UpdateAndDrawFrame, 60, 1);
        
    #else
        // Initialize variables.
        App app({ -1, -1 }, 60);
        time_point<system_clock> loopTime = system_clock::now();

        // Main loop.
        while (!WindowShouldClose())
        {
            app.Update(1.f / GetFPS());
            app.Draw();

            // Limit loop duration to target delta time.
            loopTime += milliseconds(int(app.GetTargetDeltaTime() * 1000));
            std::this_thread::sleep_until(loopTime);
        }
    #endif
    return 0;
}
