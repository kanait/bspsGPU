////////////////////////////////////////////////////////////////////
//
// $Id: GLMeshVBO.hxx 111 2005-09-23 06:45:35Z kanai $
//
//   OpenGL Mesh VBO draw class
//
// Copyright (c) 2005 by Takashi Kanai. All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _GLMESHVBO_HXX
#define _GLMESHVBO_HXX 1

#include <GL/gl.h>

#include "MeshR.hxx"
#include "GLMesh.hxx"

class GLMeshVBO : public GLMesh {

public:

  GLMeshVBO() : mesh_(NULL), drawNormal_(false), drawTexcoord_(false) {};
  ~GLMeshVBO() {};

  void setMesh( MeshR& mesh ) { mesh_ = &mesh; initVBO(); };
  MeshR& mesh() { return *mesh_; };
  bool empty() const { return ( mesh_ != NULL ) ? false : true; };

  void initVBO();
  void updateVBO();

  void clear() { if ( mesh_ ) deleteMesh(); };
  void deleteMesh() { delete mesh_; mesh_ = NULL; };

  void draw();
  void drawShading();
  void drawWireframe();

private:

  MeshR* mesh_;

  GLuint vertex_vbo_;
  GLuint normal_vbo_;
  GLuint texcoord_vbo_;
  GLuint ebo_;
  
  bool drawNormal_;
  bool drawTexcoord_;

  GLenum usage_;

};

#endif // _GLMESHVBO_HXX
