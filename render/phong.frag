#version 120

// phong.frag

varying vec4 position;
varying vec3 normal;

void main (void)
{
  // 法線ベクトル，光線ベクトル，視線ベクトル，中間ベクトル
  vec3 fnormal = normalize(normal);
  vec3 light = normalize((gl_LightSource[0].position * position.w - gl_LightSource[0].position.w * position).xyz);
  vec3 view = -normalize(position.xyz);
  vec3 halfway = normalize(light + view);

  // 拡散反射率と鏡面反射率
  float diffuse = max(dot(fnormal, light), 0.0);
  float specular = pow(max(dot(fnormal, halfway), 0.0), gl_FrontMaterial.shininess);

  // フラグメントの色
  gl_FragColor = gl_FrontLightProduct[0].ambient
               + gl_FrontLightProduct[0].diffuse * diffuse
               + gl_FrontLightProduct[0].specular * specular;
}
