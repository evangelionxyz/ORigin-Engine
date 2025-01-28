// type vertex
#version 450 core
layout(location = 0) in vec3 position;
layout (location = 4) in ivec4 bone_ids;
layout (location = 5) in vec4 weights;

const int MAX_BONES = 200;
uniform mat4 ulight_matrix;
uniform mat4 umodel_transform;
uniform mat4 ubone_transforms[MAX_BONES];
uniform bool uhas_animation;

void main()
{
    if (uhas_animation)
    {
        mat4 bone_transform = ubone_transforms[bone_ids[0]] * weights[0];
        bone_transform     += ubone_transforms[bone_ids[1]] * weights[1];
        bone_transform     += ubone_transforms[bone_ids[2]] * weights[2];
        bone_transform     += ubone_transforms[bone_ids[3]] * weights[3];
        gl_Position = ulight_matrix * umodel_transform * bone_transform * vec4(position, 1.0);
    }
    else
    {
        gl_Position = ulight_matrix * umodel_transform * vec4(position, 1.0); // transform vertex
    }
    
}

// type fragment
#version 450 core
void main() 
{
}
