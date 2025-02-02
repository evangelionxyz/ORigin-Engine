// type vertex
#version 450 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;

layout(set = 0, binding = 0) uniform UBO
{
    mat4 view_projection;
    mat4 model_transform;
} ubo;

layout(location = 0) out vec2 vtexcoord;

void main()
{
    gl_Position = ubo.view_projection * ubo.model_transform * vec4(position.x, position.y, 0.0, 1.0);
    vtexcoord = texcoord;
    
}

// type fragment
#version 450
layout(location = 0) out vec4 frag_color;
layout(location = 0) in vec2 vtexcoord;

layout(set = 0, binding = 1) uniform sampler2D textureSampler;

void main()
{
    frag_color = texture(textureSampler, vtexcoord);
}