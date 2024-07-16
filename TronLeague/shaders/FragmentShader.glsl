#version 400

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec4 ShadowCoord;

uniform sampler2D BaseTex;
uniform sampler2D ShadowMap;
uniform mat4 ViewMatrix;

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

vec3 ads(vec3 TexColor)
{
  vec4 s4 = ViewMatrix*vec4(Light.Ldir, 0.0);
  vec3 n = normalize(Normal);
  vec3 v = normalize(-Position);
  vec3 s = normalize(-vec3(s4));
  vec3 r = reflect(-s, n);
  float dRate = max(dot(s, n), 0.0);
  float sRate = pow(max(dot(r, v), 0.0), Material.Shininess);
  vec3 difusse = Light.Ld * Material.Kd * dRate;
  vec3 specular = Light.Ls * Material.Ks * sRate;
  return difusse*TexColor + specular;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{
  vec3 TexColor = vec3( texture(BaseTex,TexCoord) );
  vec3 ambient = Light.La * Material.Ka * TexColor;
  vec3 diffAndSpec = ads(TexColor);
  float ShadowCoordX = (ShadowCoord.x/ShadowCoord.w) * 0.5 + 0.5;
  float ShadowCoordY = (ShadowCoord.y/ShadowCoord.w) * 0.5 + 0.5;
  float ShadowCoordZ = (ShadowCoord.z/ShadowCoord.w) * 0.5 + 0.5;
  float shadowDepth = (texture(ShadowMap, vec2(ShadowCoordX,ShadowCoordY))).z;
  float shadow = 1.0;
  if(shadowDepth < ShadowCoordZ) shadow = 0;
  FragColor = vec4(shadow * diffAndSpec+ ambient, 1.0);
}

subroutine (RenderPassType)
void recordDepth()
{
}

void main()
{
  RenderPass();
}
