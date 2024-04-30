#version 330 core

// Interpolated values from the vertex shaders
in vec2 uv;

// Output data
out vec3 colour;

// Values that stay constant for the whole mesh
uniform sampler2D textureSampler;

void main ()
{
    // Output colour = colour of the texture at (u, v)
    colour = texture(textureSampler, uv).rgb;
}
