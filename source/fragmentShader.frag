#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

in vec3 Normal;
in vec3 fragmentPosition;

// Output data
out vec3 fragmentColour;

// Uniforms
uniform sampler2D diffuse1;
uniform vec3 lightAmbient;
uniform vec3 lightPosition;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform float Ns;


float constant = 1.0;
float linear = 0.1;
float quadratic = 0.02;

void main ()
{
    // Object colour
    vec3 objectColour = vec3(texture(diffuse1, UV));

    // Ambient reflection
    vec3 ambient = lightAmbient * objectColour;
    

    // Diffuse reflection
    vec3 normal = normalize(Normal);
    vec3 light = normalize(lightPosition - fragmentPosition);
    float cosTheta = max(dot(normal, light), 0);
    vec3 diffuse = lightDiffuse * objectColour * cosTheta;

    // Specular reflection
    vec3 eye = normalize(-fragmentPosition);
    vec3 reflection = -light + 2 * dot(light, normal) * normal;
    float cosAlpha = max(dot(eye, reflection), 0);
    vec3 specular = lightSpecular * pow(cosAlpha, Ns);

    // Attenuation
    float distance = length(lightPosition - fragmentPosition);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    // Fragment colour
    fragmentColour = (ambient + diffuse + specular) * attenuation;
}