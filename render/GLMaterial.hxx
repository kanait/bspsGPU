////////////////////////////////////////////////////////////////////
//
// $Id: GLMaterial.hxx,v 1.1.1.1 2005/04/01 17:12:52 kanai Exp $
//
//   Material setting class for OpenGL
//
// Copyright (c) 2002 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _GLMATERIAL_HXX
#define _GLMATERIAL_HXX 1

#include "envDep.h"

#ifdef __APPLE__
    #include <OpenGL/gl.h>
#elif defined(_WIN32) || defined(_WIN64)
    #include <GL/gl.h>
#else
#endif

#include "mtldata.h"

#define DEFAULT_MAT 0

static GLfloat diffuseColors[] = {
  .8f, .8f, .8f, 1.0f, // default
  .8f, .2f, .2f, 1.0f, // red
  .2f, .8f, .2f, 1.0f, // green
  .2f, .2f, .8f, 1.0f, // blue
  .8f, .8f, .2f, 1.0f, // yellow
  .8f, .2f, .8f, 1.0f, // magenta
  .2f, .8f, .8f, 1.0f, // cyan
  .2f, .2f, .2f, 1.0f, // black
  .4f, .4f, .4f, 1.0f, 
  .8f, .4f, .4f, 1.0f, 
  .4f, .8f, .4f, 1.0f, 
  .4f, .4f, .8f, 1.0f, 
  .8f, .8f, .4f, 1.0f, 
  .8f, .4f, .8f, 1.0f, 
  .4f, .8f, .8f, 1.0f
};

class GLMaterial {

public:
  
  GLMaterial() { init(); };
  GLMaterial( float* mtl ) { set( mtl ); };
  GLMaterial( float* ambient, float* diffuse, float* specular,
	    float* emission, float shininess ) {
    set( ambient, diffuse, specular, emission, shininess );
  };
  ~GLMaterial() {};

  void init() {
    set( DEFAULT_MAT );
  };
  void set( float* ambient, float* diffuse, float* specular,
	    float* emission, float shininess ) {
    int i;
    for ( i = 0; i < 4; ++i ) ambient_[i] = ambient[i];
    for ( i = 0; i < 4; ++i ) diffuse_[i] = diffuse[i];
    for ( i = 0; i < 4; ++i ) specular_[i] = specular[i];
    for ( i = 0; i < 4; ++i ) emission_[i] = emission[i];
    shininess_[0] = shininess;
  };

  void set( float* mtl ) {
    ambient_[0] = mtl[0];
    ambient_[1] = mtl[1];
    ambient_[2] = mtl[2];
    ambient_[3] = mtl[3];
    diffuse_[0] = mtl[4];
    diffuse_[1] = mtl[5];
    diffuse_[2] = mtl[6];
    diffuse_[3] = mtl[7];
    emission_[0] = mtl[8];
    emission_[1] = mtl[9];
    emission_[2] = mtl[10];
    emission_[3] = mtl[11];
    specular_[0] = mtl[12];
    specular_[1] = mtl[13];
    specular_[2] = mtl[14];
    specular_[3] = mtl[15];
    shininess_[0] = mtl[16];
  };
    
  // set materials from database
  void set( int no ) {
    int id = no * NUM_MTL_ITEMS;
    ambient_[0] = (GLfloat) mtlall[id+0];
    ambient_[1] = (GLfloat) mtlall[id+1];
    ambient_[2] = (GLfloat) mtlall[id+2];
    ambient_[3] = (GLfloat) mtlall[id+3];
    diffuse_[0] = (GLfloat) mtlall[id+4];
    diffuse_[1] = (GLfloat) mtlall[id+5];
    diffuse_[2] = (GLfloat) mtlall[id+6];
    diffuse_[3] = (GLfloat) mtlall[id+7];
    emission_[0] = (GLfloat) mtlall[id+8];
    emission_[1] = (GLfloat) mtlall[id+9];
    emission_[2] = (GLfloat) mtlall[id+10];
    emission_[3] = (GLfloat) mtlall[id+11];
    specular_[0] = (GLfloat) mtlall[id+12];
    specular_[1] = (GLfloat) mtlall[id+13];
    specular_[2] = (GLfloat) mtlall[id+14];
    specular_[3] = (GLfloat) mtlall[id+15];
    shininess_[0] = (GLfloat) mtlall[id+16];
  };
  float* getDiffuseColor() { return &(diffuse_[0]); };
  void setDiffuseColor( float r, float g, float b, float a ) {
    diffuse_[0] = r;
    diffuse_[1] = g;
    diffuse_[2] = b;
    diffuse_[3] = a;
    ::glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse_ );
  };

  void setDiffuseColor( int n ) {
    
    int i = n; if ( n > 14 ) i = 14;
    diffuse_[0] = diffuseColors[ 4 * i ];
    diffuse_[1] = diffuseColors[ 4 * i + 1 ];
    diffuse_[2] = diffuseColors[ 4 * i + 2 ];
    diffuse_[3] = diffuseColors[ 4 * i + 3 ];
    ::glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse_ );
  };
    
  void bind() {
    ::glMaterialfv( GL_FRONT, GL_AMBIENT, ambient_ );
    ::glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse_ );
    ::glMaterialfv( GL_FRONT, GL_SPECULAR, specular_ );
    ::glMaterialfv( GL_FRONT, GL_EMISSION, emission_ );
    ::glMaterialfv( GL_FRONT, GL_SHININESS, shininess_ );
  };

private:

  GLfloat ambient_[4];
  GLfloat diffuse_[4];
  GLfloat emission_[4];
  GLfloat specular_[4];
  GLfloat shininess_[1];
  
};

#endif // _GLMATERIAL_H
