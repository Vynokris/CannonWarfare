#version 100

precision highp float;

// Input vertex attributes (from vertex shader).
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values.
uniform sampler2D texture0;
uniform vec4      colDiffuse;
uniform vec2      screenSize;

// Variables.
const vec2  pixelSize  = vec2(1.0) / screenSize;
const float blurRadius = 5.0;

void main()
{
    vec4  sum       = vec4(0.0);
    float coeffSum  = 0.0;
    
    // Do a wheighted average of the surrounding pixels.
    for (float x = -blurRadius; x <= blurRadius; x++) {
        for (float y = -blurRadius; y <= blurRadius; y++)
        {
            sum      += (blurRadius - abs(x) + blurRadius - abs(y) + 1.0) * texture2D(texture0, fragTexCoord + vec2(x, y) * pixelSize);
            coeffSum += (blurRadius - abs(x) + blurRadius - abs(y) + 1.0);
        }
    }
    sum /= coeffSum;

    // Calculate final fragment color.
    gl_FragColor = vec4(sum.rgb, 1.0) * colDiffuse;
}