#version 450

layout(location = 0) in vec3 fragNorm;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragColor;
layout(location = 3) in vec3 fragPos;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(set = 0, binding = 1) uniform LightingUBO
{
    vec4 lightColor;
    vec4 lightDir;
    vec4 cameraPos;

    int lightingEnabled;
    int texturesEnabled;
} lighting;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 baseColor = vec4(fragColor, 1.0);

    if (lighting.texturesEnabled != 0) {
        baseColor *= texture(texSampler, fragTexCoord);
    }

    if (lighting.lightingEnabled == 0) {
        outColor = baseColor;
        return;
    }

    vec3 N = normalize(fragNorm);
    vec3 L = normalize(-lighting.lightDir.xyz);
    vec3 V = normalize(lighting.cameraPos.xyz - fragPos);
    vec3 H = normalize(L + V);

    float ambientStrength = 0.05;
    vec3 ambient = ambientStrength * baseColor.rgb;

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * baseColor.rgb * lighting.lightColor.rgb;

    float specularStrength = 0.6; 
    float shininess = 32.0;
    float spec = pow(max(dot(N, H), 0.0), shininess);
    vec3 specular = specularStrength * spec * lighting.lightColor.rgb;

    vec3 lightingResult = ambient + diffuse + specular;

    outColor = vec4(lightingResult, baseColor.a);
}