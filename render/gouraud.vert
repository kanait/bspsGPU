#version 120

// gouraud.vert

void main(void)
{
  // ���_�ʒu�C�@���x�N�g���C�����x�N�g���C�����x�N�g���C���ԃx�N�g��
  vec4 position = gl_ModelViewMatrix * gl_Vertex;
  vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
  vec3 light = normalize((gl_LightSource[0].position * position.w - gl_LightSource[0].position.w * position).xyz);
  vec3 view = -normalize(position.xyz);
  vec3 halfway = normalize(light + view);

  // �g�U���˗��Ƌ��ʔ��˗�
  float diffuse = max(dot(light, normal), 0.0);
  float specular = pow(max(dot(normal, halfway), 0.0), gl_FrontMaterial.shininess);

  // ���_�̐F
  gl_FrontColor = gl_FrontLightProduct[0].ambient
                + gl_FrontLightProduct[0].diffuse * diffuse
                + gl_FrontLightProduct[0].specular * specular;

  // ���_�ʒu
  gl_Position = ftransform();
}
