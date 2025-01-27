// type vertex
#version 450 core
layout(location = 0) in vec3 position;

const int MAX_CASCADES = 3;
uniform mat4 ulight_matrices[MAX_CASCADES]; // array of light-space matrices
uniform int ucascade_index;                 // active cascade index
uniform mat4 umodel_transform;                 // object's model matrix

void main()
{
    mat4 light_matrix = ulight_matrices[ucascade_index];              // select light-space matrix
    gl_Position = light_matrix * umodel_transform * vec4(position, 1.0); // transform vertex
}

// type fragment
#version 450 core
void main() 
{
  // no color output; only depth is written to the shadow map
}
