#version 120

// phong.frag

varying vec4 position;
varying vec3 normal;

void main (void)
{
  // �@���x�N�g���C�����x�N�g���C�����x�N�g���C���ԃx�N�g��
  vec3 fnormal = normalize(normal);
  vec3 light = normalize((gl_LightSource[0].position * position.w - gl_LightSource[0].position.w * position).xyz);
  vec3 view = -normalize(position.xyz);
  vec3 halfway = normalize(light + view);

  // �g�U���˗��Ƌ��ʔ��˗�
  float diffuse = max(dot(fnormal, light), 0.0);
  float specular = pow(max(dot(fnormal, halfway), 0.0), gl_FrontMaterial.shininess);

  // �t���O�����g�̐F
  gl_FragColor = gl_FrontLightProduct[0].ambient
               + gl_FrontLightProduct[0].diffuse * diffuse
               + gl_FrontLightProduct[0].specular * specular;
}
