// MSDF text shader

// type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityID;

layout (std140, binding = 0) uniform UBO
{
	mat4 ViewProjection;
	mat4 LightSpaceMatrix;
	vec3 CameraPosition;

} uboData;

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

	gl_Position = uboData.ViewProjection * vec4(a_Position, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

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
	
	switch(int(v_TexIndex))
	{
		case  0: texColor *= texture(u_FontAtlases[ 0], Input.TexCoord); msd = texture(u_FontAtlases[ 0], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[ 0])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case  1: texColor *= texture(u_FontAtlases[ 1], Input.TexCoord); msd = texture(u_FontAtlases[ 1], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[ 1])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case  2: texColor *= texture(u_FontAtlases[ 2], Input.TexCoord); msd = texture(u_FontAtlases[ 2], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[ 2])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case  3: texColor *= texture(u_FontAtlases[ 3], Input.TexCoord); msd = texture(u_FontAtlases[ 3], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[ 3])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case  4: texColor *= texture(u_FontAtlases[ 4], Input.TexCoord); msd = texture(u_FontAtlases[ 4], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[ 4])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case  5: texColor *= texture(u_FontAtlases[ 5], Input.TexCoord); msd = texture(u_FontAtlases[ 5], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[ 5])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case  6: texColor *= texture(u_FontAtlases[ 6], Input.TexCoord); msd = texture(u_FontAtlases[ 6], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[ 6])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case  7: texColor *= texture(u_FontAtlases[ 7], Input.TexCoord); msd = texture(u_FontAtlases[ 7], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[ 7])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case  8: texColor *= texture(u_FontAtlases[ 8], Input.TexCoord); msd = texture(u_FontAtlases[ 8], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[ 8])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case  9: texColor *= texture(u_FontAtlases[ 9], Input.TexCoord); msd = texture(u_FontAtlases[ 9], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[ 9])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 10: texColor *= texture(u_FontAtlases[10], Input.TexCoord); msd = texture(u_FontAtlases[10], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[10])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 11: texColor *= texture(u_FontAtlases[11], Input.TexCoord); msd = texture(u_FontAtlases[11], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[11])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 12: texColor *= texture(u_FontAtlases[12], Input.TexCoord); msd = texture(u_FontAtlases[12], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[12])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 13: texColor *= texture(u_FontAtlases[13], Input.TexCoord); msd = texture(u_FontAtlases[13], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[13])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 14: texColor *= texture(u_FontAtlases[14], Input.TexCoord); msd = texture(u_FontAtlases[14], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[14])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 15: texColor *= texture(u_FontAtlases[15], Input.TexCoord); msd = texture(u_FontAtlases[15], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[15])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 16: texColor *= texture(u_FontAtlases[16], Input.TexCoord); msd = texture(u_FontAtlases[16], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[16])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 17: texColor *= texture(u_FontAtlases[17], Input.TexCoord); msd = texture(u_FontAtlases[17], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[17])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 18: texColor *= texture(u_FontAtlases[18], Input.TexCoord); msd = texture(u_FontAtlases[18], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[18])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 19: texColor *= texture(u_FontAtlases[19], Input.TexCoord); msd = texture(u_FontAtlases[19], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[19])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 20: texColor *= texture(u_FontAtlases[20], Input.TexCoord); msd = texture(u_FontAtlases[20], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[20])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 21: texColor *= texture(u_FontAtlases[21], Input.TexCoord); msd = texture(u_FontAtlases[21], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[21])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 22: texColor *= texture(u_FontAtlases[22], Input.TexCoord); msd = texture(u_FontAtlases[22], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[22])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 23: texColor *= texture(u_FontAtlases[23], Input.TexCoord); msd = texture(u_FontAtlases[23], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[23])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 24: texColor *= texture(u_FontAtlases[24], Input.TexCoord); msd = texture(u_FontAtlases[24], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[24])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 25: texColor *= texture(u_FontAtlases[25], Input.TexCoord); msd = texture(u_FontAtlases[25], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[25])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 26: texColor *= texture(u_FontAtlases[26], Input.TexCoord); msd = texture(u_FontAtlases[26], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[26])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 27: texColor *= texture(u_FontAtlases[27], Input.TexCoord); msd = texture(u_FontAtlases[27], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[27])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 28: texColor *= texture(u_FontAtlases[28], Input.TexCoord); msd = texture(u_FontAtlases[28], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[28])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 29: texColor *= texture(u_FontAtlases[29], Input.TexCoord); msd = texture(u_FontAtlases[29], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[29])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 30: texColor *= texture(u_FontAtlases[30], Input.TexCoord); msd = texture(u_FontAtlases[30], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[30])*(median(msd.r, msd.g, msd.b)- 0.5); break;
		case 31: texColor *= texture(u_FontAtlases[31], Input.TexCoord); msd = texture(u_FontAtlases[31], Input.TexCoord).rgb; screenPxDistance = screenPxRange(u_FontAtlases[31])*(median(msd.r, msd.g, msd.b)- 0.5); break;
	}

    
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	if (opacity == 0.0)
		discard;

	vec4 bgColor = vec4(0.0);
    o_Color = mix(bgColor, Input.Color, opacity);
	if (o_Color.a == 0.0)
		discard;

	o_EntityID = v_EntityID;
}