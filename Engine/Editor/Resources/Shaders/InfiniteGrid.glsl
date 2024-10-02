// type vertex
#version 450 core

const vec3 positions[4] = vec3[4](
  vec3(-1.0, 0.0, -1.0),
  vec3( 1.0, 0.0, -1.0),
  vec3( 1.0, 0.0,  1.0),
  vec3(-1.0, 0.0,  1.0)
);

const int indices[6] = int[6](0, 1, 2, 0, 2, 3);

uniform mat4 viewProjection;
uniform vec3 cameraPosition;

const float gridSize = 400.0;

layout(location = 0) out vec3 vPosition;
layout(location = 1) out vec3 vCamPos;
layout(location = 2) out float vGridSize;


void main()
{
  int index = indices[gl_VertexID];
  vec3 pos = positions[index] * gridSize;
  pos.x += cameraPosition.x;
  pos.z += cameraPosition.z;
  vec4 vPos = vec4(pos, 1.0);
  gl_Position = viewProjection * vPos;

  vPosition = pos;
  vGridSize = gridSize;
  vCamPos = cameraPosition;
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vCamPos;
layout(location = 2) in float vGridSize;

float gridMinPixelsBetweenCells = 2.0;
float gridCellSize = 0.025;
vec4 gridColorThin = vec4(0.5, 0.5, 0.5, 0.8);
vec4 gridColorThick = vec4(0.0, 0.0, 0.0, 0.8);

float log10(float x) {return log(x) / log(10.0);}
float satf(float x) { return clamp(x, 0.0, 1.0);}
vec2 satv(vec2 x) { return clamp(x, vec2(0.0), vec2(1.0));}
float max2(vec2 v) { return max(v.x, v.y);}

void main()
{
  vec2 dvx = vec2(dFdx(vPosition.x), dFdy(vPosition.x));
  vec2 dvy = vec2(dFdx(vPosition.z), dFdy(vPosition.z));
  float lx = length(dvx);
  float ly = length(dvy);

  vec2 dudv = vec2(lx, ly);
  float l = length(dudv);

  float LOD = max(0.0, log10(l * gridMinPixelsBetweenCells / gridCellSize) + 1.0);
  float gridCellSizeLod0 = gridCellSize * pow(10.0, floor(LOD));
  float gridCellSizeLod1 = gridCellSizeLod0 * 10.0;
  float gridCellSizeLod2 = gridCellSizeLod1 * 10.0;

  dudv *= 4.0;

  vec2 mod_div_dudv = mod(vPosition.xz, gridCellSizeLod0) / dudv;
  float lod0a = max2(vec2(1.0) - abs( satv(mod_div_dudv) * 2.0 - vec2(1.0)) );

  mod_div_dudv = mod(vPosition.xz, gridCellSizeLod1) / dudv;
  float lod1a = max2(vec2(1.0) - abs( satv(mod_div_dudv) * 2.0 - vec2(1.0)) );

  mod_div_dudv = mod(vPosition.xz, gridCellSizeLod2) / dudv;
  float lod2a = max2(vec2(1.0) - abs( satv(mod_div_dudv) * 2.0 - vec2(1.0)) );

  float LOD_Fade = fract(LOD);

  vec4 color;

  if (lod2a > 0.0)
  {
    color = gridColorThick;
    color.a *= lod2a;
  } else {
    if (lod1a > 0.0) {
      color = mix(gridColorThick, gridColorThin, LOD_Fade);
      color.a *= lod1a;
    } else {
      color = gridColorThin;
      color.a *= lod0a * (1.0 - LOD_Fade);
    }
  }

  float falloff = (1.0 - satf(length(vPosition.xz - vCamPos.xz) / (vGridSize * 0.7)));
  color.a *= falloff;

  oColor = color;
}