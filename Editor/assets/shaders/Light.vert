#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 BiTangent;

// Imports the camera matrix from the main function
uniform mat4 camMatrix;
// Imports the model matrix from the main function
uniform mat4 model;

void main()
{
    gl_Position = camMatrix * model * vec4(aPos, 1.0);
}