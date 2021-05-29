#version 120

// gouraud.vert

void main(void)
{
  // 頂点位置，法線ベクトル，光線ベクトル，視線ベクトル，中間ベクトル
  vec4 position = gl_ModelViewMatrix * gl_Vertex;
  vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
  vec3 light = normalize((gl_LightSource[0].position * position.w - gl_LightSource[0].position.w * position).xyz);
  vec3 view = -normalize(position.xyz);
  vec3 halfway = normalize(light + view);

  // 拡散反射率と鏡面反射率
  float diffuse = max(dot(light, normal), 0.0);
  float specular = pow(max(dot(normal, halfway), 0.0), gl_FrontMaterial.shininess);

  // 頂点の色
  gl_FrontColor = gl_FrontLightProduct[0].ambient
                + gl_FrontLightProduct[0].diffuse * diffuse
                + gl_FrontLightProduct[0].specular * specular;

  // 頂点位置
  gl_Position = ftransform();
}
