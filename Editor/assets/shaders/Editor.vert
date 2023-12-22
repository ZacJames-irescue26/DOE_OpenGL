#version 450 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
//layout (location = 1) in vec3 aColor;
// Normals (not necessarily normalized)
layout (location = 1) in vec3 aNormal;
// Texture Coordinates
layout (location = 2) in vec2 aTex;
layout(location = 3) in int EntityID;

// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the Fragment Shader
out vec2 texCoord;
// Outputs the normal for the Fragment Shader
out vec3 Normal;
// Outputs the current position for the Fragment Shader
out vec3 crntPos;
// Outputs the fragment position of the light
out vec4 fragPosLight;
out flat int v_EntityID;



// Imports the camera matrix from the main function
uniform mat4 camMatrix;
// Imports the model matrix from the main function
uniform mat4 model;
// Imports the light matrix
uniform mat4 lightSpaceMatrix;

void main()
{
	 v_EntityID = EntityID;
	// calculates current position
	crntPos = vec3(model * vec4(aPos, 1.0f));
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos, 1.0);

	// Assigns the colors from the Vertex Data to "color"
	//color = aColor;
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = aTex;
	// Assigns the normal from the Vertex Data to "Normal"
	Normal = aNormal;
	fragPosLight = lightSpaceMatrix * vec4(crntPos, 1.0f);
}