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
const float intensity  = 5.0;

void main()
{
    // Variables proportianal to screen size.
    vec2 pixelSize  = vec2(1.0) / screenSize;
    vec2 vignette   = 1100.0 * pixelSize;

    // Normal and modified pixel colors.
    vec4 fragColor = texture2D(texture0, fragTexCoord);
    vec4 newColor;

    // Distance from the center of the screen.
    vec2 distCenter = vec2(sqrt(pow(0.5 - fragTexCoord.x, 2.0)),
                           sqrt(pow(0.5 - fragTexCoord.y, 2.0)));

    // Get the red and blue values of neighboor pixels.
    newColor.r = texture2D(texture0, fragTexCoord + vec2( intensity, 0.0) * pixelSize).r;
    newColor.g = texture2D(texture0, fragTexCoord                                    ).g;
    newColor.b = texture2D(texture0, fragTexCoord + vec2(-intensity, 0.0) * pixelSize).b;
    newColor.a = 1.0;

    // If the pixel is on the exterior of the vignette circle, apply maximum chromatic aberration.
    if (distCenter.x > vignette.x && distCenter.y > vignette.y)
    {
        gl_FragColor = newColor;
    }

    // If not, apply chromatic aberration depending on distance drom the center.    
    else
    {
        float ratio = (distCenter.x/vignette.x + distCenter.y/vignette.y) / 2.0;
        gl_FragColor = newColor * ratio + fragColor * (1.0 - ratio);
    }
}