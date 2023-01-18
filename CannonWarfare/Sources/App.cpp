#include "App.h"
#include "RaylibConversions.h"
#include <rlImGui.h>

using namespace Maths;


App::App(const Maths::Vector2& _screenSize, const int& _targetFPS)
    : screenSize(_screenSize), targetFPS(_targetFPS), targetDeltaTime(1.f / targetFPS), cannon(this, groundHeight)
{
    startTime = std::chrono::system_clock::now();

	// Initialize Raylib.
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

    // Set the ground height.
    groundHeight = screenSize.y - 100;

    // Set the cannon's default position, rotation and shooting velocity.
    cannon.SetPosition({ 90, screenSize.y - 150 });
    cannon.SetRotation(-PI / 5);
    cannon.SetShootingVelocity(900);

    particleManager = new ParticleManager();
}

App::~App()
{
    ImGui::SaveIniSettingsToDisk("Resources/imgui.ini");
    ShutdownRLImGui();
    CloseWindow();
}

float App::GetTimeSinceStart()
{
	const std::chrono::system_clock::time_point curTime = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(curTime - App::startTime).count() / 1000.f;
}


void App::Update(const float& deltaTime)
{
    cannon.Update(deltaTime);
    particleManager->Update(deltaTime);
}

void App::Draw()
{
    BeginDrawing();
    {
        ClearBackground(BLACK);
        cannon.DrawTrajectories();
        cannon.Draw();
        DrawRectangle(0, (int)groundHeight, (int)screenSize.x, (int)(screenSize.y - groundHeight), BLACK);
        cannon.DrawMeasurements();
        DrawLine(0, (int)groundHeight, (int)screenSize.x, (int)groundHeight, WHITE);
        particleManager->Draw();
        DrawUi();
    }
    EndDrawing();
}

void App::DrawUi()
{
    BeginRLImGui();
    {
        // Stats window.
        if (ImGui::Begin("Stats", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
        {
            ImGui::SetWindowPos({ screenSize.x - ImGui::GetWindowWidth(), 1 });
            const int fps = GetFPS();
            ImGui::Text("FPS: %d", fps);
            ImGui::Text("Delta Time: %.3f", 1.f / fps);
        }
        ImGui::End();

        // Cannon window.
        if (ImGui::Begin("Cannon", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::PushItemWidth(43);

            static float shootingVelocity = cannon.GetShootingVelocity();
            if (ImGui::DragFloat("Shooting Velocity", &shootingVelocity, 10.f, 100.f, 2000.f, "%.0f"))
                cannon.SetShootingVelocity(shootingVelocity);

            static float height = 0;
            if (ImGui::DragFloat("Height", &height, 1.f, 0.f, screenSize.y - 250, "%.0f"))
                cannon.SetPosition({ cannon.GetPosition().x, screenSize.y - 150 - height });

            static float rotation = -radToDeg(cannon.GetRotation());
            if (!cannon.automaticRotation && ImGui::DragFloat("Rotation", &rotation, 1.f, -90.f, 90.f, "%.0f"))
                cannon.SetRotation(-degToRad(rotation));

            if (ImGui::Checkbox("Automatic rotation", &cannon.automaticRotation))
                rotation = -radToDeg(cannon.GetRotation());

            if (ImGui::Button("Shoot"))
                cannon.Shoot();

            ImGui::SameLine();
            if (ImGui::Button("Clear"))
                cannon.ClearProjectiles();

            ImGui::PopItemWidth();
        }
        ImGui::End();

        // Trajectory window.
        if (ImGui::Begin("Trajectory", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Air time: %.2f seconds",        cannon.GetAirTime());
            ImGui::Text("Landing distance: %.0f pixels", cannon.GetLandingDistance());
            ImGui::Text("Maximum height: %.0f pixels",   cannon.GetMaxHeight());
        }
        ImGui::End();
    }
    EndRLImGui();
}
