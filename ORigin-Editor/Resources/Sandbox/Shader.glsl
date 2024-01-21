// type vertex
#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 viewProjection;
uniform mat4 modelTransform;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

void main()
{
	fragTexCoord = texCoord;
  fragPosition = vec3(modelTransform * vec4(position, 1.0));
  mat3 normalMatrix = transpose(inverse(mat3(modelTransform)));
  fragNormal = normalize(normalMatrix * normal);

	gl_Position = viewProjection * modelTransform * vec4(fragPosition, 1.0);
}

// type fragment
#version 450 core
layout(location = 0) out vec4 oColor;

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

uniform sampler2D u_DiffTexture;
uniform samplerCube u_DepthTexture;

uniform vec3 cameraPosition;
uniform float farPlane;

struct Light
{
  vec3 Position;
  vec3 Color;
  float Size;
  float Specular;
  float Ambient;
};
uniform Light lights[1];

vec3 pointLight(vec3 fragPosition, vec3 fragNormal, vec2 fragTexCoord)
{
  vec3 viewDirection = normalize(cameraPosition - fragPosition);
  vec3 normal = normalize(fragNormal);
  vec3 lightDirection = normalize(lights[0].Position - fragPosition);

  float distanceToLight = length(lights[0].Position - fragPosition);  
  float attenuation = 1.0 / (1.0 + pow(distanceToLight / lights[0].Size, 2.0));

  vec3 ambient = lights[0].Ambient * lights[0].Color;
  vec3 diffuse = max(dot(normal, lightDirection), 0.0) * attenuation * lights[0].Color;

  vec3 reflectionDirection = reflect(-lightDirection, normal);
  float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), 16.0);
  vec3 specular = spec * attenuation * lights[0].Specular * lights[0].Color;

  float shadow  = 0.0;
  float bias    = 0.05;
  float samples = 2.0;
  vec3 lightToFrag = fragPosition - lights[0].Position;
  float currentDepth = length(lightToFrag);
  for(float x = -samples; x < samples; x++)
  {
    for(float y = -samples; y < samples; y++)
    {
      for(float z = -samples; z < samples; z++)
      {
        float closestDepth = texture(u_DepthTexture, lightToFrag + vec3(x, y, z)).r; 
        closestDepth *= farPlane;

        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
      }
    }
  }
  shadow /= (samples * samples * samples);

  return ambient + (1.0 - shadow) * (diffuse + specular);
}

void main()
{
  vec3 tex = texture(u_DiffTexture, fragTexCoord).rgb;
  vec3 finalColor = pointLight(fragPosition, fragNormal, fragTexCoord);
  oColor = vec4(tex * finalColor, 1.0);
}