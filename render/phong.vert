#version 120

// phong.vert

varying vec4 position;
varying vec3 normal;

void main(void)
{
  // 頂点位置，法線ベクトル
  position = gl_ModelViewMatrix * gl_Vertex;
  normal = normalize(gl_NormalMatrix * gl_Normal);

  // 頂点位置
  gl_Position = ftransform();
}
