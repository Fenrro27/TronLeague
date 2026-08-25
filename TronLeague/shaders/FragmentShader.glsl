#version 400

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec4 ShadowCoord;

uniform sampler2D BaseTex;
uniform sampler2D ShadowMap;
uniform mat4 ViewMatrix;
uniform int UseTexture;

struct LightInfo {
  vec3 Ldir;
  vec3 La;
  vec3 Ld;
  vec3 Ls;
};
uniform LightInfo Light;

struct MaterialInfo{
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float Shininess;
};
uniform MaterialInfo Material;

out vec4 FragColor;

vec3 ads(vec3 TexColor, vec3 s, vec3 n)
{
  vec3 v = normalize(-Position);
  vec3 r = reflect(-s, n);
  float dRate = max(dot(s, n), 0.0);
  float sRate = 0.0;
  if (dRate > 0.0) {
    float shininess = max(Material.Shininess, 1.0);
    sRate = pow(max(dot(r, v), 0.0), shininess);
  }
  vec3 diffuse = Light.Ld * Material.Kd * dRate * TexColor;
  vec3 specular = Light.Ls * Material.Ks * sRate;
  return diffuse + specular;
}

float calculateShadow(vec3 n, vec3 s)
{
  // Perspectiva / coordenadas normalizadas del espacio de sombra
  vec3 projCoords = ShadowCoord.xyz / ShadowCoord.w;
  projCoords = projCoords * 0.5 + 0.5;

  // Si está fuera del frustum de la luz, no hay sombra
  if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
    return 1.0;

  // Bias adaptativo según el ángulo de incidencia para eliminar shadow acne
  float bias = max(0.005 * (1.0 - dot(n, s)), 0.001);
  float currentDepth = projCoords.z;

  // Filtrado PCF (Percentage-Closer Filtering) 3x3 para bordes suaves
  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(ShadowMap, 0);
  for(int x = -1; x <= 1; ++x)
  {
    for(int y = -1; y <= 1; ++y)
    {
      float pcfDepth = texture(ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
      shadow += (currentDepth - bias > pcfDepth) ? 0.0 : 1.0;
    }
  }
  shadow /= 9.0;

  return shadow;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{
  vec3 TexColor = (UseTexture == 1) ? texture(BaseTex, TexCoord).rgb : vec3(1.0);
  vec3 ambient = Light.La * Material.Ka * TexColor;

  vec4 s4 = ViewMatrix * vec4(Light.Ldir, 0.0);
  vec3 s = normalize(-vec3(s4));
  vec3 n = normalize(Normal);

  vec3 diffAndSpec = ads(TexColor, s, n);
  float shadow = calculateShadow(n, s);

  FragColor = vec4(ambient + shadow * diffAndSpec, 1.0);
}

subroutine (RenderPassType)
void recordDepth()
{
}

void main()
{
  RenderPass();
}
