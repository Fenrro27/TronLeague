 #version 400

 layout(location = 0) in vec3 VertexPosition;

 uniform mat4 Inverse;

 out vec3 Position;


 void main()
 {
     gl_Position = vec4(VertexPosition,1.0);
     Position = vec3(Inverse * gl_Position);
     Position = normalize(Position);
 }
