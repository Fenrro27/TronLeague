#version 400

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

uniform mat4 MVP;
uniform mat4 ViewMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ShadowMatrix;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;
out vec4 ShadowCoord;

void main()
{
  vec4 n4 = ModelViewMatrix*vec4(VertexNormal, 0.0);
  vec4 v4 = ModelViewMatrix*vec4(VertexPosition, 1.0);
  Normal = vec3(n4);
  Position = vec3(v4);
  TexCoord = VertexTexCoord;
  ShadowCoord = ShadowMatrix * vec4(VertexPosition, 1.0);
  gl_Position = MVP * vec4(VertexPosition, 1.0);
}