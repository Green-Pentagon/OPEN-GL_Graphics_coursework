#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 fragmentPosition;
in vec3 Normal;

// Output data
out vec3 fragmentColour;

// Structs
struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float linear, constant, quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cosPhi;
    float linear, constant, quadratic;
};

struct DirLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Uniforms
#define maxLights 50
uniform sampler2D diffuse1;
uniform Light lights[maxLights];
uniform int numLights;
uniform float Ns;
uniform SpotLight spotLights[maxLights];
uniform int numSpotLights;
uniform DirLight dirLight;

// Function prototypes
vec3 calculatePointLight(Light ptLight, vec3 fragmentPosition, vec3 normal, vec3 eye);
vec3 calculateSpotLight(SpotLight spotLight, vec3 fragmentPosition, vec3 normal, vec3 eye);
vec3 calculateDirectionalLight(DirLight dirLight, vec3 normal, vec3 eye);

void main ()
{
    // Calculate normal and eye vectors (these are the same for all light sources)
    vec3 normal = normalize(Normal);
    vec3 eye = normalize(-fragmentPosition);
    
    // Loop through the point light sources
    fragmentColour = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < numLights; i++)
    {
        fragmentColour += calculatePointLight(lights[i], fragmentPosition, normal, eye);
    }
    
    // Loop through the spot light sources
    for (int i = 0; i < numSpotLights; i++)
    {
        fragmentColour += calculateSpotLight(spotLights[i], fragmentPosition, normal, eye);
    }
    
    // Calculate directional light
    fragmentColour += calculateDirectionalLight(dirLight, normal, eye);
    
}

// Calculate point light
vec3 calculatePointLight(Light ptLight, vec3 fragmentPosition, vec3 normal, vec3 eye)
{
    // Object colour
    vec3 objectColour = vec3(texture(diffuse1, UV));
    
    // Ambient reflection
    vec3 ambient = ptLight.ambient * objectColour;
    
    // Diffuse reflection
    vec3 light = normalize(ptLight.position - fragmentPosition);
    float cosTheta = max(dot(normal, light), 0);
    vec3 diffuse = ptLight.diffuse * objectColour * cosTheta;
    
    // Specular reflection
    vec3 reflection = -light + 2 * dot(light, normal) * normal;
    float cosAlpha = max(dot(eye, reflection), 0);
    vec3 specular = ptLight.specular * pow(cosAlpha, Ns);
    
    // Attenuation
    float distance = length(ptLight.position - fragmentPosition);
    float attenuation = 1.0 / (ptLight.constant + ptLight.linear * distance + ptLight.quadratic * distance * distance);
    
    // Return fragment colour
    return (ambient + diffuse + specular) * attenuation;
}

// Calculate spot light
vec3 calculateSpotLight(SpotLight spotLight, vec3 fragmentPosition, vec3 normal, vec3 eye)
{
    // Object colour
    vec3 objectColour = vec3(texture(diffuse1, UV));
    
    // Ambient reflection
    vec3 ambient = spotLight.ambient * objectColour;
    
    // Diffuse reflection
    vec3 light = normalize(spotLight.position - fragmentPosition);
    float cosTheta = max(dot(normal, light), 0);
    vec3 diffuse = spotLight.diffuse * objectColour * cosTheta;
    
    // Specular reflection
    vec3 reflection = -light + 2 * dot(light, normal) * normal;
    float cosAlpha = max(dot(eye, reflection), 0);
    vec3 specular = spotLight.specular * pow(cosAlpha, Ns);
    
    // Attenuation
    float distance = length(spotLight.position - fragmentPosition);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * distance * distance);
    
    // Spotlight intensity
    vec3 direction = normalize(spotLight.direction);
    cosTheta = dot(light, -direction);
    float delta = radians(2);
    float intensity = clamp((cosTheta - spotLight.cosPhi) / delta, 0.0, 1.0);
    
    // Return fragment colour
    return ambient * attenuation + (diffuse + specular) * attenuation * intensity;
}

// Calculate directional light
vec3 calculateDirectionalLight(DirLight dirLight, vec3 normal, vec3 eye)
{
    // Object colour
    vec3 objectColour = vec3(texture(diffuse1, UV));
    
    // Ambient reflection
    vec3 ambient = dirLight.ambient * objectColour;
    
    // Diffuse refection
    vec3 light = normalize(-dirLight.direction);
    float cosTheta = max(dot(normal, light), 0);
    vec3 diffuse = dirLight.diffuse * objectColour * cosTheta;
    
    // Specular reflection
    vec3 reflection = -light + 2 * dot(light, normal) * normal;
    float cosAlpha = max(dot(eye, reflection), 0);
    vec3 specular = dirLight.specular * pow(cosAlpha, Ns);
    
    // Return fragment colour
    return ambient + diffuse + specular;
}
