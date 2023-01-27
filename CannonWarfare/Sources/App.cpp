#include "App.h"
#include "Graphics.h"
#include "RaylibConversions.h"
#include <rlImGui.h>

using namespace Maths;


App::App(const Maths::Vector2& _screenSize, const int& _targetFPS)
    : screenSize(_screenSize), targetFPS(_targetFPS), targetDeltaTime(1.f / targetFPS), cannon(particleManager, groundHeight)
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
    ImGui::GetIO().IniFilename       = NULL;
    ImGui::LoadIniSettingsFromDisk("Resources/imgui.ini");

    /*
    // Can be used to get wheel callbacks on web.
    #if defined PLATFORM_WEB
        emscripten_set_wheel_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, [](int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData) -> int { static_cast<Ui*>(userData)->mouseWheelMove = wheelEvent->deltaY; return 1; });
    #endif
    */

    // Initialize the app graphics.
    graphics = new Graphics(screenSize);

    // Initialize the stars.
    stars.reserve(STAR_COUNT);
    for (size_t i = 0; i < STAR_COUNT; ++i)
        stars.emplace_back(screenSize);

    // Set the ground height.
    groundHeight = screenSize.y - 100;

    // Set the cannon's default position, rotation and shooting velocity.
    cannon.SetPosition ({ 90, screenSize.y - 150 });
    cannon.SetAnchorPos({ 90, screenSize.y - 150 });
    cannon.SetRotation(-PI / 5);
}

App::~App()
{
    delete graphics;
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
    for (Star& star : stars)
        star.Update(deltaTime);
    cannon.Update(deltaTime);
    particleManager.Update(deltaTime);
}

void App::Draw()
{
    graphics->BeginDrawing();
    {
        for (const Star& star : stars) star.Draw(); // Draw stars.
        particleManager.Draw();
        cannon.DrawTrajectories();
        cannon.Draw();
        DrawRectangle(0, (int)groundHeight, (int)screenSize.x, (int)(screenSize.y - groundHeight), BLACK); // Draw ground.
        cannon.DrawMeasurements();
        DrawLine(0, (int)groundHeight, (int)screenSize.x, (int)groundHeight, WHITE); // Draw ground top.
        DrawUi();
    }
    graphics->EndDrawing();
}

void App::DrawUi()
{
    BeginRLImGui();
    {
        // Stats window.
        if (ImGui::Begin("Stats", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::SetWindowPos({ screenSize.x - ImGui::GetWindowWidth(), 1 });
            
            ImGui::Checkbox("Show predicted trajectory", &cannon.showTrajectory);
            ImGui::Checkbox("Show predicted measurements", &cannon.showMeasurements);
            ImGui::Checkbox("Show cannonball trajectories", &cannon.showProjectileTrajectories);
            
            const int fps = GetFPS();
            ImGui::Text("FPS: %d | Delta Time: %.2f", fps, 1.f / fps);
        }
        ImGui::End();

        // Cannon window.
        if (ImGui::Begin("Cannon", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::SetWindowPos({ screenSize.x / 2.f - ImGui::GetWindowWidth() / 2.f, 1.f });
            ImGui::PushItemWidth(43);

            static float powderCharge = cannon.GetPowderCharge();
            if (ImGui::DragFloat("Powder Charge (kg)", &powderCharge, 0.1f, 2, 10, "%.1f"))
                cannon.SetPowderCharge(clamp(powderCharge, 2, 10));

            static float barrelLength = cannon.GetBarrelLength();
            if (ImGui::DragFloat("Barrel Length (px)", &barrelLength, 2, 500, 2500, "%.0f"))
                cannon.SetBarrelLength(clamp(barrelLength, 500, 2500));

            static float projectileRadius = cannon.GetProjectileRadius();
            if (ImGui::DragFloat("Projectile Radius (px)", &projectileRadius, 0.5f, 5, 50, "%.0f"))
                cannon.SetProjectileRadius(clamp(projectileRadius, 5, 50));

            static float projectileMass = cannon.GetProjectileMass();
            if (ImGui::DragFloat("Projectile Mass (kg)", &projectileMass, 0.2f, 2, 50, "%.1f"))
                cannon.SetProjectileMass(clamp(projectileMass, 2, 50));

            static float height = 0;
            if (ImGui::DragFloat("Height (px)", &height, 0.5f, 0.f, screenSize.y - 250, "%.0f")) {
                cannon.SetAnchorPos({ cannon.GetAnchorPos().x, screenSize.y - 150 - height });
                cannon.SetPosition (cannon.GetAnchorPos());
            }

            static float rotation = -radToDeg(cannon.GetRotation());
            if (!cannon.automaticRotation && ImGui::DragFloat("Rotation (deg)", &rotation, 0.1f, -89.9f, 89.9f, "%.1f"))
                cannon.SetRotation(-degToRad(clamp(rotation, -89.9f, 89.9f)));

            if (ImGui::Checkbox("Automatic rotation", &cannon.automaticRotation))
                rotation = -radToDeg(cannon.GetRotation());

            if (ImGui::Button("Shoot"))
                cannon.Shoot();

            ImGui::SameLine();
            if (ImGui::Button("Clear"))
                cannon.ClearProjectiles();

            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                const CannonProperties defaults;
                cannon.SetPowderCharge(defaults.powderCharge);
                cannon.SetBarrelLength(defaults.barrelLength);
                cannon.SetProjectileRadius(defaults.projectileRadius);
                cannon.SetProjectileMass(defaults.projectileMass);
                powderCharge     = defaults.powderCharge;
                barrelLength     = defaults.barrelLength;
                projectileRadius = defaults.projectileRadius;
                projectileMass   = defaults.projectileMass;
            }

            ImGui::PopItemWidth();
        }
        ImGui::End();

        // Trajectory window.
        if (ImGui::Begin("Trajectory", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Air time: %.2f seconds",        cannon.GetAirTime());
            ImGui::Text("Landing distance: %.0f pixels", cannon.GetLandingDistance());
            ImGui::Text("Maximum height: %.0f pixels",   cannon.GetMaxHeight());
            
            ImGui::Checkbox("Apply recoil", &cannon.applyRecoil);
            if (ImGui::Checkbox("Apply drag", &cannon.applyDrag)) {
                cannon.applyCollisions = false;
                cannon.SetRotation(cannon.GetRotation());
            }
            if (ImGui::Checkbox("Apply collisions", &cannon.applyCollisions)) {
                cannon.applyDrag = false;
            }
        }
        ImGui::End();
    }
    EndRLImGui();
}
