#pragma once
#include "Vector2.h"
#include "raylib.h"

class Graphics
{
private:
    const Maths::Vector2& screenSize;

    // Screen shake.
    Maths::Vector2 ShakeOffset = { 0, 0 };

    // Shaders.
    Shader gaussianBlurShader = {};
    Shader nonBlackMaskShader = {};
    Shader chromaticAberrationShader = {};
    int blurDirLocation = 0;
    int bloomIntensity  = 5;

    // Render textures.
    RenderTexture2D renderTexture = {};
    RenderTexture2D blurTextures[2] = {{},{}};

public:
    bool mouseCursorHidden = false;

private:
    void ApplyBlur() const;

public:
    Graphics(const Maths::Vector2& _screenSize);

    void BeginDrawing() const;
    void EndDrawing() const;
};