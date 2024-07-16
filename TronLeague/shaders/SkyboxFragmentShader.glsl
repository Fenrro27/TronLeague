#version 400

 in vec3 Position;

 out vec4 FragColor;

 uniform samplerCube CubemapTex;

 void main()
 {
    FragColor = texture(CubemapTex,Position);
 }
