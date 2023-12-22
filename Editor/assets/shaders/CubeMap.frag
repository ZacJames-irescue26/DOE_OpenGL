#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform samplerCube environmentMap;
uniform bool isEnvironmentMap;
void main()
{
    if(isEnvironmentMap == true)
    {
        vec3 envColor = texture(environmentMap, TexCoords).rgb;
    
        envColor = envColor / (envColor + vec3(1.0));
        envColor = pow(envColor, vec3(1.0/2.2)); 
  
        FragColor = vec4(envColor, 1.0);
    }
    
    else
    {
        FragColor = texture(skybox, TexCoords);
    }
}