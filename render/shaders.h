////////////////////////////////////////////////////////////////////
//
// $Id: $
//
// Copyright (c) 2015 by Takashi Kanai. All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _SHADERS_H
#define _SHADERS_H 1

// same as texture.vert
static const GLchar *vertex_shader_Texture_source =
  "#version 120\n"
  "varying vec4 position;"
  "varying vec3 normal;"
  "void main(void)"
  "{"
  "position = gl_ModelViewMatrix * gl_Vertex;"
  "normal = normalize(gl_NormalMatrix * gl_Normal);"
  "gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
  "gl_Position = ftransform();"
  "}\0";

// same as texture.frag
static const GLchar *fragment_shader_Texture_source =
  "#version 120\n"
  "uniform sampler2D texture;"
  "varying vec4 position;"
  "varying vec3 normal;"
  "void main (void)"
  "{"
  "vec4 color = texture2DProj(texture, gl_TexCoord[0]);"
  "vec3 fnormal = normalize(normal);"
  "vec3 light = normalize((gl_LightSource[0].position * position.w - gl_LightSource[0].position.w * position).xyz);"
  "vec3 view = -normalize(position.xyz);"
  "vec3 halfway = normalize(light + view);"
  "float diffuse = max(dot(fnormal, light), 0.0);"
  "float specular = pow(max(dot(fnormal, halfway), 0.0), gl_FrontMaterial.shininess);"
  "gl_FragColor = gl_LightSource[0].ambient * color + gl_LightSource[0].diffuse * diffuse * color + gl_FrontLightProduct[0].specular * specular;"
  "}\0";

// same as phong.vert
static const GLchar *vertex_shader_Phong_source =
  "#version 120\n"
  "varying vec4 position;"
  "varying vec3 normal;"
  "void main(void)"
  "{"
  "position = gl_ModelViewMatrix * gl_Vertex;"
  "normal = normalize(gl_NormalMatrix * gl_Normal);"
  "gl_Position = ftransform();"
  "}\0";

// same as phong.frag
static const GLchar *fragment_shader_Phong_source =
  "#version 120\n"
  "varying vec4 position;"
  "varying vec3 normal;"
  "void main (void)"
  "{"
  "vec3 fnormal = normalize(normal);"
  "vec3 light = normalize((gl_LightSource[0].position * position.w - gl_LightSource[0].position.w * position).xyz);"
  "vec3 view = -normalize(position.xyz);"
  "vec3 halfway = normalize(light + view);"
  "float diffuse = max(dot(fnormal, light), 0.0);"
  "float specular = pow(max(dot(fnormal, halfway), 0.0), gl_FrontMaterial.shininess);"
  "gl_FragColor = gl_FrontLightProduct[0].ambient + gl_FrontLightProduct[0].diffuse * diffuse + gl_FrontLightProduct[0].specular * specular;"
  "}\0";

static const GLchar *vertex_shader_Gourand_source =
  "#version 120\n"
  "void main(void)"
  "{"
  "vec4 position = gl_ModelViewMatrix * gl_Vertex;"
  "vec3 normal = normalize(gl_NormalMatrix * gl_Normal);"
  "vec3 light = normalize((gl_LightSource[0].position * position.w - gl_LightSource[0].position.w * position).xyz);"
  "vec3 view = -normalize(position.xyz);"
  "vec3 halfway = normalize(light + view);"
  "float diffuse = max(dot(light, normal), 0.0);"
  "float specular = pow(max(dot(normal, halfway), 0.0), gl_FrontMaterial.shininess);"
  "gl_FrontColor = gl_FrontLightProduct[0].ambient + gl_FrontLightProduct[0].diffuse * diffuse + gl_FrontLightProduct[0].specular * specular;"
  "gl_Position = ftransform();"
  "}\0";

static const GLchar *fragment_shader_Gourand_source =
  "#version 120\n"
  "void main (void)"
  "{"
  "gl_FragColor = gl_Color;"
  "}\0";

static const GLchar *vertex_wireframe_source =
  "#version 120\n"
  "varying vec4 position;"
  "void main(void)"
  "{"
  "position = gl_ModelViewMatrix * gl_Vertex;"
  "gl_Position = ftransform();"
  "gl_FrontColor = gl_Color;"
  "}\0";

static const GLchar *fragment_wireframe_source =
  "#version 120\n"
  "void main (void)"
  "{"
  "gl_FragColor = gl_Color;"
  "}\0";

#define PHONG_SHADING   0
#define GOURAND_SHADING 1
#define WIREFRAME       2
#define PHONG_TEXTURE   3

#endif // _SHADERS_H
