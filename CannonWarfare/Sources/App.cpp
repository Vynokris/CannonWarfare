#include "App.h"
#include "RaylibConversions.h"
#include <rlImGui.h>
using namespace Maths;

App::App(const Maths::Vector2& _screenSize, const int& _targetFPS, const RGBA& _clearColor)
    : screenSize(_screenSize), clearColor(_clearColor), targetFPS(_targetFPS), targetDeltaTime(1.f / targetFPS), cannon(groundHeight)
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

    // Set the cannon's default position, rotation and shooting velocity.
    cannon.position = { 60, screenSize.y - 120 };
    cannon.rotation = -PI / 5;
    cannon.shootingVelocity = 1000;

    // Set the ground height.
    groundHeight = screenSize.y - 60;
}

App::~App()
{
    ImGui::SaveIniSettingsToDisk("Resources/imgui.ini");
    ShutdownRLImGui();
    CloseWindow();
}


void App::Update(const float& deltaTime)
{
    cannon.Update(deltaTime);
}

void App::Draw()
{
    BeginDrawing();
    {
        ClearBackground(ToRayColor(clearColor));
        DrawLine(0, groundHeight, screenSize.x, groundHeight, WHITE);
        cannon.Draw();

        DrawUi();
    }
    EndDrawing();
}

void App::DrawUi()
{
    BeginRLImGui();
    {
        // Stats window.
        if (ImGui::Begin("Stats"))
        {
            const int fps = GetFPS();
            ImGui::Text("FPS: %d", fps);
            ImGui::Text("Delta Time: %.3f", 1.f / fps);
        }
        ImGui::End();

        // Cannon window.
        if (ImGui::Begin("Cannon"))
        {
            ImGui::PushItemWidth(100);

            static float height = 0;
            if (ImGui::DragFloat("Height", &height, 1.f, 0.f, screenSize.y - 240))
                cannon.position.y = screenSize.y - 120 - height;

            static float rotation = -radToDeg(cannon.rotation);
            if (ImGui::DragFloat("Rotation", &rotation, 1.f, 0.f, 90.f))
                cannon.rotation = -degToRad(rotation);

            ImGui::DragFloat("Shooting Velocity", &cannon.shootingVelocity, 10.f, 100.f, 2000.f);

            if (ImGui::Button("Shoot"))
                cannon.Shoot();

            ImGui::PopItemWidth();
        }
        ImGui::End();
    }
    EndRLImGui();
}
