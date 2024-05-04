// type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityID;

layout (std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
	vec3 Position;

} CameraBuffer;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};

layout (location = 0) out VertexOutput Output;
layout (location = 2) out flat float v_TexIndex;
layout (location = 3) out flat int v_EntityID;

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_EntityID = a_EntityID;

	gl_Position = CameraBuffer.ViewProjection * vec4(a_Position, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;
layout(location = 1) out int oEntityID;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};

layout (location = 0) in VertexOutput Input;
layout (location = 2) in flat float v_TexIndex;
layout (location = 3) in flat int v_EntityID;

layout (binding = 0) uniform sampler2D u_FontAtlases[32];

float screenPxRange(sampler2D atlas) {
	const float pxRange = 2.0; // set to distance field's pixel range
  vec2 unitRange = vec2(pxRange)/vec2(textureSize(atlas, 0));
  vec2 screenTexSize = vec2(1.0)/fwidth(Input.TexCoord);
  return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) {
   return max(min(r, g), min(max(r, g), b));
}

void main()
{
	vec3 msd = vec3(0.0);
	float screenPxDistance = 0.0f;
	vec4 texColor = Input.Color;
	
	texColor *= texture(u_FontAtlases[int(v_TexIndex)], Input.TexCoord);
	msd = texture(u_FontAtlases[int(v_TexIndex)], Input.TexCoord).rgb;
	screenPxDistance = screenPxRange(u_FontAtlases[int(u_FontAtlases)])*(median(msd.r, msd.g, msd.b) - 0.5);
    
  float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	if (opacity == 0.0)
		discard;

	vec4 bgColor = vec4(0.0);
    oColor = mix(bgColor, Input.Color, opacity);
	if (oColor.a == 0.0)
		discard;

	oEntityID = v_EntityID;
}