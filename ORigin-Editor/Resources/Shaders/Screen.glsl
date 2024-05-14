// type vertex
#version 460 core
layout(location = 0) in vec2 aPos;
layout(location = 0) out vec2 vTexCoord;

uniform mat4 uViewProjection;

void main()
{
    gl_Position = uViewProjection * vec4(aPos, 0.0, 1.0);
    vTexCoord = vec2((aPos.x + 1.0) / 2.0, (aPos.y + 1.0) / 2.0);
}

// type fragment
#version 460 core
layout(location = 0) out vec4 oColor;
layout(location = 0) in vec2 vTexCoord;

uniform sampler2D uScreenTexture;

void main()
{
    oColor = texture(uScreenTexture, vTexCoord);

    if (oColor.a <= 0.0)
      discard;
}