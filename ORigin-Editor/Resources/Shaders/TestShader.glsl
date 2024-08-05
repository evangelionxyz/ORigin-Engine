// type vertex
#version 450 core
layout (location = 0) in vec3 aPosition;

uniform mat4 viewProjection;
uniform mat4 model;

void main()
{
    gl_Position = viewProjection * model * vec4(aPosition, 1.0);
}


// type fragment
#version 450 core
layout (location = 0) out vec4 oColor;
layout (location = 1) out int oEntityID;

uniform int entityId;

void main()
{
    oColor = vec4(1.0, 1.0, 1.0, 1.0);
    oEntityID = entityId;
}