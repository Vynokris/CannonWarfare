#version 100

precision highp float;

// Input vertex attributes (from vertex shader).
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values.
uniform sampler2D texture0;
uniform vec4      colDiffuse;
uniform vec2      screenSize;
uniform int       isVertical;

// Constant variables.
const float blurRadius = 4.0;

void main()
{
    // Variables proportional to screen size.
    vec2 pixelSize = vec2(1.0) / screenSize;

    // Variables used to to the wheighted average.
    vec4  sum       = vec4(0.0);
    float coeffSum  = 0.0;
    
    // Do a vertical/horizontal wheighted average of the surrounding pixels.
    for (float i = -blurRadius; i < blurRadius; i++)
    {
        vec2 curPos;
        if (isVertical == 1) { curPos = vec2(0, i); }
        else                 { curPos = vec2(i, 0); }

        sum      += (blurRadius - abs(i) + 1.0) * texture2D(texture0, fragTexCoord + curPos * pixelSize);
        coeffSum += (blurRadius - abs(i) + 1.0);
    }
    sum /= coeffSum;

    // Calculate final fragment color.
    gl_FragColor = vec4(sum.rgb, 1.0) * colDiffuse;
}