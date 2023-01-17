#include "App.h"
#include <raylib.h>
#include <rlImGui.h>

App::App(const Maths::Vector2& _screenSize, const int& _targetFPS)
    : screenSize(_screenSize), targetFPS(_targetFPS), targetDeltaTime(1.f / targetFPS)
{
	// Initialize raylib.
    InitWindow(screenSize.x <= 0 ? 1728 : (int)screenSize.x, screenSize.y <= 0 ? 972 : (int)screenSize.y, "Cannon Warfare");
    SetTargetFPS(targetFPS);

    // Set window size to the monitor size.
    if (screenSize.x <= 0 || screenSize.y <= 0)
    {
        screenSize = { (float)GetMonitorWidth(0), (float)GetMonitorHeight(0) };
        SetWindowSize((int)screenSize.x, (int)screenSize.y);
        SetWindowPosition(0, 30);
    }

    // Setup ImGui.
    SetupRLImGui(true);
    ImGui::GetStyle().WindowRounding = 5.0;
    ImGui::LoadIniSettingsFromDisk("Resources/imgui.ini");

    /*
    // Can be used to get wheel callbacks on web.
    #if defined PLATFORM_WEB
        emscripten_set_wheel_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, [](int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData) -> int { static_cast<Ui*>(userData)->mouseWheelMove = wheelEvent->deltaY; return 1; });
    #endif
    */
}

App::~App()
{
    ImGui::SaveIniSettingsToDisk("Resources/imgui.ini");
    ShutdownRLImGui();
    CloseWindow();
}


void App::Update(const float& deltaTime)
{

}

void App::Draw()
{
    BeginDrawing();
    {
        ClearBackground(DARKGRAY);
        DrawRectangle(10, 10, 100, 100, BLUE);

        DrawUi();
    }
    EndDrawing();
}

void App::DrawUi()
{
    BeginRLImGui();
    {
        // Stats window.
        if (ImGui::Begin("Stats"));
        {
            const int fps = GetFPS();
            ImGui::Text("FPS: %d", fps);
            ImGui::Text("Delta Time: %.3f", 1.f / fps);
        }
        ImGui::End();
    }
    EndRLImGui();
}
