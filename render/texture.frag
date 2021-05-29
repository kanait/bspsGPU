#version 120

// texture.frag

uniform sampler2D texture;

varying vec4 position;
varying vec3 normal;

void main (void)
{
  // �e�N�X�`�������f�̐F�𓾂�
  vec4 color = texture2DProj(texture, gl_TexCoord[0]);

  // �@���x�N�g���C�����x�N�g���C�����x�N�g���C���ԃx�N�g��
  vec3 fnormal = normalize(normal);
  vec3 light = normalize((gl_LightSource[0].position * position.w - gl_LightSource[0].position.w * position).xyz);
  vec3 view = -normalize(position.xyz);
  vec3 halfway = normalize(light + view);

  // �g�U���˗��Ƌ��ʔ��˗�
  float diffuse = max(dot(fnormal, light), 0.0);
  float specular = pow(max(dot(fnormal, halfway), 0.0), gl_FrontMaterial.shininess);

  // �t���O�����g�̐F
  gl_FragColor = gl_LightSource[0].ambient * color
               + gl_LightSource[0].diffuse * diffuse * color
               + gl_FrontLightProduct[0].specular * specular;
}
