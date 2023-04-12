#version 330 core

in vec3 FragPos;  
in vec3 Normal; 
in vec2 TexCoords;

uniform sampler2D maintexture;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 1
#define MAX_POINT_LIGHTS 128
uniform PointLight pointLight[128];
uniform int pointLightsAmount;

out vec4 FragColor;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir); 

void main()
{
    //FragColor = texture(maintexture, TexCoords);

    // property
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: direction light
    //vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: point light
    vec3 result = vec3(0.0);
    for(int i = 0; i < pointLightsAmount; i++){
        result += CalcPointLight(pointLight[i], norm, FragPos, viewDir);  
    }
     

    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    FragColor = vec4(result, 1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 rayDir = normalize(light.position - fragPos);
    // diffuse light
    float diff = max(dot(normal, rayDir), 0.0);
    // specular light
    vec3 reflectDir = reflect(-rayDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // fading
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combining results
    vec3 ambient  = material.ambient  * vec3(texture(maintexture, TexCoords));
    vec3 diffuse  = material.diffuse  * diff * vec3(texture(maintexture, TexCoords));
    vec3 specular = material.specular * spec * vec3(texture(maintexture, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (lightColor * (ambient + diffuse + specular));
} 
