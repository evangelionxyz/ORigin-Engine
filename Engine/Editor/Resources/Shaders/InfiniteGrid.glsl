// type vertex
#version 450 core

const vec3 positions[4] = vec3[4](
  vec3(-1.0, 0.0, -1.0),
  vec3( 1.0, 0.0, -1.0),
  vec3( 1.0, 0.0,  1.0),
  vec3(-1.0, 0.0,  1.0)
);

const int indices[6] = int[6](0, 1, 2, 0, 2, 3);

layout(std140, binding = 0) uniform Camera
{
  mat4 view_projection;
  vec3 position;
} camera_buffer;

const float gridSize = 300.0;

layout(location = 0) out Vertex
{
    vec3 position;
    float grid_size;
} vout;


void main()
{
    int index = indices[gl_VertexID];
    vec3 pos = positions[index] * gridSize;
    pos.x += camera_buffer.position.x;
    pos.z += camera_buffer.position.z;
    gl_Position = camera_buffer.view_projection * vec4(pos, 1.0);
    vout.position = pos;
    vout.grid_size = gridSize;
}

// type fragment
#version 450 core
layout(location = 0) out vec4 frag_color;

layout(std140, binding = 0) uniform Camera
{
  mat4 view_projection;
  vec3 position;
} camera_buffer;

layout(location = 0) in Vertex
{
    vec3 position;
    float grid_size;
} vin;

float grid_cell_size = 0.25;
float grid_min_pixels_between_cells = 10.0;
vec4 uthin_color = vec4(0.789, 0.789, 0.789, 1.0);
vec4 uthick_color = vec4(0.456, 0.456, 0.456, 1.0);

float log10(float x) {return log(x) / log(10.0);}
float satf(float x) { return clamp(x, 0.0, 1.0);}
vec2 satv(vec2 x) { return clamp(x, vec2(0.0), vec2(1.0));}
float max2(vec2 v) { return max(v.x, v.y);}

void main()
{
    vec2 dvx = vec2(dFdx(vin.position.x), dFdy(vin.position.x));
    vec2 dvy = vec2(dFdx(vin.position.z), dFdy(vin.position.z));
    float lx = length(dvx);
    float ly = length(dvy);

    vec2 dudv = vec2(lx, ly);
    float l = length(dudv);

    float LOD = max(0.0, log10(l * grid_min_pixels_between_cells / grid_cell_size) + 1.0);
    float grid_cell_size_lod_0 = grid_cell_size * pow(8.0, floor(LOD));
    float grid_cell_size_lod_1 = grid_cell_size_lod_0 * 8.0;
    float grid_cell_size_lod_2 = grid_cell_size_lod_1 * 8.0;

    dudv *= 2.0;

    vec2 mod_div_dudv = mod(vin.position.xz, grid_cell_size_lod_0) / dudv;
    float lod0a = max2(vec2(1.0) - abs( satv(mod_div_dudv) * 2.0 - vec2(1.0)) );

    mod_div_dudv = mod(vin.position.xz, grid_cell_size_lod_1) / dudv;
    float lod1a = max2(vec2(1.0) - abs( satv(mod_div_dudv) * 2.0 - vec2(1.0)) );

    mod_div_dudv = mod(vin.position.xz, grid_cell_size_lod_2) / dudv;
    float lod2a = max2(vec2(1.0) - abs( satv(mod_div_dudv) * 2.0 - vec2(1.0)) );

    float LOD_Fade = fract(LOD);

    vec4 color;

    if (lod2a > 0.0)
    {
        color = uthick_color;
        color.a *= lod2a;
    } else {
        if (lod1a > 0.0) {
              color = mix(uthick_color, uthin_color, LOD_Fade);
              color.a *= lod1a;
        } else {
            color = uthin_color;
            color.a *= lod0a * (1.0 - LOD_Fade);
        }
    }

    float falloff = 1.0 - smoothstep(0.7, 1.0, length(vin.position.xz - camera_buffer.position.xz) / (vin.grid_size * 0.8));

    color.a *= falloff;
    if (color.a < 0.1) discard;

    frag_color = color;
}