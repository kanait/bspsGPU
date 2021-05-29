#version 120

// texture.vert

varying vec4 position;
varying vec3 normal;

void main(void)
{
  // ���_�ʒu�C�@���x�N�g��
  position = gl_ModelViewMatrix * gl_Vertex;
  normal = normalize(gl_NormalMatrix * gl_Normal);

  // �e�N�X�`�����W�C���_�ʒu
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
  gl_Position = ftransform();
}
