#version 450 core

// Outputs colors in RGBA
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityId;
//out vec4 FragColor;

// Imports the color from the Vertex Shader
//in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the fragment position of the light
in vec4 fragPosLight;

in flat int v_EntityID;

//uniform samplerCube skybox;

// Gets the Texture Units from the main function
uniform sampler2D texture_diffuse0;
uniform sampler2D specular0;
uniform sampler2D shadowMap;
struct Light {
    vec3 Position;
    vec3 Color;
};

uniform Light lights[16];

// Gets the color of the light from the main function
uniform vec4 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;
// Gets the position of the camera from the main function
uniform vec3 camPos;


vec4 PointLight()
{
       
  vec3 color = texture(texture_diffuse0, texCoord).rgb;
    vec3 normal = normalize(Normal);
    // ambient
    vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < 16; i++)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - crntPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = lights[i].Color * diff * color;      
        vec3 result = diffuse;        
        // attenuation (use quadratic as we have gamma correction)
        float distance = length(crntPos - lights[i].Position);
        result *= 1.0 / (distance * distance);
        lighting += result;
                
    }
	return vec4(ambient + lighting, 1.0);
}

vec4 direcLight()
{
		// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	};


	// Shadow value
	float shadow = 0.0f;
	// Sets lightCoords to cull space
	vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
	if(lightCoords.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;
		// Prevents shadow acne
		float bias = max(0.025f * (1.0f - dot(normal, lightDirection)), 0.0005f);

		// Smoothens out the shadows
		int sampleRadius = 2;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;     
		    }    
		}
		// Get average shadow
		shadow /= pow((sampleRadius * 2 + 1), 2);

	}

	return (texture(texture_diffuse0, texCoord) * (diffuse * (1.0f - shadow) + ambient) + texture(specular0, texCoord).r * specular  * (1.0f - shadow)) * lightColor;
	
}

vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.0020f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	};

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(texture_diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor;
}
float inverseLerp(float v, float minvalue, float maxvalue)
{
    return (v - minvalue) / (maxvalue - minvalue);
}

float remap(float v, float inMin, float inMax, float outMin, float outMax)
{
    float t = inverseLerp(v, inMin, inMax);
    return mix(outMin, outMax, v);
}


void main()
{
	// outputs final color
	//FragColor = direcLight();
	//vec3 I = normalize(Position - camPos);
    //vec3 R = reflect(I, normalize(Normal));
    //FragColor = vec4(texture(skybox, R).rgb, 1.0);
	vec3 baseColour = vec3(0.75);
	vec3 lighting = vec3(0.0);
	vec3 Colour = vec3(0.0);
	vec3 normal = normalize(Normal);
	// ambient
	vec3 Ambient = vec3(0.5);

	//hemi Light


	lighting = Ambient;

	Colour = baseColour * lighting;
	//Colour = normal;
	FragColor = vec4(Colour, 1.0f);
	EntityId = v_EntityID;
}