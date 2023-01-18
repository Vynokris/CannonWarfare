#include "Graphics.h"
#include "RaylibConversions.h"

Graphics::Graphics(const Maths::Vector2& _screenSize)
    : screenSize(_screenSize)
{
    // Adjust the bloom intensity according to screen size.
    bloomIntensity = (int)(bloomIntensity * screenSize.x / 2560.f);

    // Load shaders.
    gaussianBlurShader        = LoadShader(NULL, "Resources/Shaders/GaussianBlur.fs");
    nonBlackMaskShader        = LoadShader(NULL, "Resources/Shaders/NonBlackPixels.fs");
    chromaticAberrationShader = LoadShader(NULL, "Resources/Shaders/ChromaticAberration.fs");
    renderTexture             = LoadRenderTexture((int)screenSize.x, (int)screenSize.y);
    blurTextures[0]           = LoadRenderTexture((int)screenSize.x, (int)screenSize.y);
    blurTextures[1]           = LoadRenderTexture((int)screenSize.x, (int)screenSize.y);
    blurDirLocation           = GetShaderLocation(gaussianBlurShader, "isVertical");

    // Set gaussian blur shader screen size.
    {
        const int screenSizeLocation = GetShaderLocation(gaussianBlurShader, "screenSize");
        SetShaderValue(gaussianBlurShader, screenSizeLocation, &screenSize, SHADER_UNIFORM_VEC2);
    }

    // Set chromatic aberration shader screen size.
    {
        const int screenSizeLocation = GetShaderLocation(chromaticAberrationShader, "screenSize");
        SetShaderValue(chromaticAberrationShader, screenSizeLocation, &screenSize, SHADER_UNIFORM_VEC2);
    }
}

void Graphics::BeginDrawing() const
{
    if (mouseCursorHidden)
        HideCursor();
    BeginTextureMode(renderTexture);
    ClearBackground(BLACK);
}

void Graphics::EndDrawing() const
{
    EndTextureMode();
    ApplyBlur();

    ::BeginDrawing();
    {
        ClearBackground(BLACK);
        
        BeginShaderMode(chromaticAberrationShader);
        {
            // Draw the blurred texture.
            DrawTextureRec(blurTextures[0].texture,
                           Rectangle{0, 0, screenSize.x, -screenSize.y},
                           ToRayVector2(ShakeOffset),
                           WHITE);
            
            // Draw the non-black pixels of the original texture.
            BeginShaderMode(nonBlackMaskShader);
            { 
                DrawTextureRec(renderTexture.texture,
                               Rectangle{0, 0, screenSize.x, -screenSize.y},
                               ToRayVector2(ShakeOffset),
                               WHITE);
            }
            EndShaderMode();
        }
        EndShaderMode();
    }
    ::EndDrawing();
}

void Graphics::ApplyBlur() const
{
    for (int i = 0; i < bloomIntensity; i++)
    {
        // Draw the render texture on the first blurring texture.
        BeginTextureMode(blurTextures[0]);
        {
            if (i == 0)
                ClearBackground(BLACK);
            BeginShaderMode(nonBlackMaskShader);
            DrawTextureRec(renderTexture.texture,
                           Rectangle{0, 0, screenSize.x, -screenSize.y},
                           ::Vector2{0, 0},
                           WHITE);
            EndShaderMode();
        }
        EndTextureMode();

        // Blur the 2 blurring render textures (ping pong texturing).
        for (int j = 0; j < 2; j++)
        { 
            BeginTextureMode(blurTextures[(j+1)%2]);
            {
                ClearBackground(BLACK);
                BeginShaderMode(gaussianBlurShader);
                SetShaderValue(gaussianBlurShader, blurDirLocation, &j, SHADER_UNIFORM_INT);
                DrawTextureRec(blurTextures[j].texture,
                               Rectangle{0, 0, screenSize.x, -screenSize.y},
                               ::Vector2{0, 0},
                               WHITE);
                EndShaderMode();
            }
            EndTextureMode();
        }
    }
}
