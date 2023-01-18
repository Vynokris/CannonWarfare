#version 100

precision mediump float;

// Input vertex attributes (from vertex shader).
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values.
uniform sampler2D texture0;
uniform vec4      colDiffuse;

void main()
{
    // Add the two textures together.
    if (texture2D(texture0, fragTexCoord).rgb != vec4(0).rgb)
        gl_FragColor = texture2D(texture0, fragTexCoord);
    else
        gl_FragColor = vec4(0);
}