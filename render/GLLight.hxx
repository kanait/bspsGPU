////////////////////////////////////////////////////////////////////
//
// $Id: GLLight.hxx,v 1.2 2002/12/25 08:11:37 kanai Exp $
//
//   Light class for OpenGL
//
// Copyright (c) 2002 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _GLLIGHT_HXX
#define _GLLIGHT_HXX 1

#include "envDep.h"

#ifdef __APPLE__
    #include <OpenGL/gl.h>
#elif defined(_WIN32) || defined(_WIN64)
    #include <GL/gl.h>
#else
#endif


#define NUM_LIGHT 8

static int light_id_[] = {
    GL_LIGHT0,
    GL_LIGHT1,
    GL_LIGHT2,
    GL_LIGHT3,
    GL_LIGHT4,
    GL_LIGHT5,
    GL_LIGHT6,
    GL_LIGHT7
  };

// static float default_light_[] = {
//   .0f, .0f, 100.0f, 0.0f,
//   .0f, .0f, .0f, 1.0f,
//   1.0f, 1.0f, 1.0f, 1.0f,
//   1.0f, 1.0f, 1.0f, 1.0f,
//   1.0f,
//   0.0f
// };

static float default_light_[] = {
  0.0f, 0.0f, 100.0f, 1.0f,
  0.8f, 0.8f, 0.8f, 1.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  0.8f, 0.8f, 0.8f, 1.0f,
  1.0f, // 0.1f,
  0.0f,  // 0.05f
  0.0f  // 0.05f
};

static float light_position_[] = {
  0.0f, 0.0f, 10.0f, 0.0f, 
  0.0f, 0.0f, -10.0f, 0.0f, 
  10.0f, 0.0f, 0.0f, 0.0f, 
  -10.0f, 0.0f, 0.0f, 0.0f
};

class GLLight {

public:

  GLLight(){};
  GLLight( int id ) {
    set( id );
  };
  GLLight( int id, float* pos ) {
    set( id );
    setPos( pos );
  };
  ~GLLight(){};

  void init() {
    position_[0] = default_light_[0];
    position_[1] = default_light_[1];
    position_[2] = default_light_[2];
    position_[3] = default_light_[3];
    ambient_[0]  = default_light_[4];
    ambient_[1]  = default_light_[5];
    ambient_[2]  = default_light_[6];
    ambient_[3]  = default_light_[7];
    diffuse_[0]  = default_light_[8];
    diffuse_[1]  = default_light_[9];
    diffuse_[2]  = default_light_[10];
    diffuse_[3]  = default_light_[11];
    specular_[0] = default_light_[12];
    specular_[1] = default_light_[13];
    specular_[2] = default_light_[14];
    specular_[3] = default_light_[15];
    const_attenuation_  = default_light_[16];
    linear_attenuation_ = default_light_[17];
    quadratic_attenuation_ = default_light_[18];
  };

  void set( int id ) {
    setID( id );
    init();
    bind();
    on();
  };
  void setID( int id ) { id_ = id; };

  void setPos( float* pos ) {
    setPos( pos[0], pos[1], pos[2], pos[3] );
  };
  void setPos( float x, float y, float z, float w ) {
    position_[0] = x; position_[1] = y;
    position_[2] = z; position_[3] = w;
    ::glLightfv( light_id_[id_], GL_POSITION, position_ );
  };

  void getPos( float* x, float* y, float* z, float* w ) {
    *x = position_[0];
    *y = position_[1];
    *z = position_[2];
    *w = position_[3];
  };

  void set( GLfloat* light ) {
    position_[0] = light[0];
    position_[1] = light[1];
    position_[2] = light[2];
    position_[3] = light[3];
    ambient_[0]  = light[4];
    ambient_[1]  = light[5];
    ambient_[2]  = light[6];
    ambient_[3]  = light[7];
    diffuse_[0]  = light[8];
    diffuse_[1]  = light[9];
    diffuse_[2]  = light[10];
    diffuse_[3]  = light[11];
    specular_[0] = light[12];
    specular_[1] = light[13];
    specular_[2] = light[14];
    specular_[3] = light[15];
    const_attenuation_     = light[16];
    linear_attenuation_    = light[17];
    quadratic_attenuation_ = light[18];

    bind();
  };

  void set( GLfloat* position, GLfloat* ambient, GLfloat* diffuse,
	    GLfloat* specular, GLfloat const_attenuation,
	    GLfloat linear_attenuation, GLfloat quadratic_attenuation ) {
    int i;
    for ( i = 0; i < 4; ++i ) position_[i] = position[i];
    for ( i = 0; i < 4; ++i ) ambient_[i] = ambient[i];
    for ( i = 0; i < 4; ++i ) diffuse_[i] = diffuse[i];
    for ( i = 0; i < 4; ++i ) specular_[i] = specular[i];
    const_attenuation_     = const_attenuation;
    linear_attenuation_    = linear_attenuation;
    quadratic_attenuation_ = quadratic_attenuation;

    bind();
  };
    
  void bind() {
    ::glLightfv( light_id_[id_], GL_POSITION, position_ );
    ::glLightfv( light_id_[id_], GL_AMBIENT,  ambient_ );
    ::glLightfv( light_id_[id_], GL_DIFFUSE,  diffuse_ );
    ::glLightfv( light_id_[id_], GL_SPECULAR, specular_ );
    ::glLightf( light_id_[id_], GL_CONSTANT_ATTENUATION, const_attenuation_ );
    ::glLightf( light_id_[id_], GL_LINEAR_ATTENUATION, linear_attenuation_ );
    ::glLightf( light_id_[id_], GL_QUADRATIC_ATTENUATION, quadratic_attenuation_ );
  };

  void setIsOn( bool f ) { 
    isOn_ = f;
    if ( f == true ) on();
    else off();
  };
  bool isOn() const { return isOn_; };

  void on() { ::glEnable( light_id_[id_] ); };
  void off() { ::glDisable( light_id_[id_] ); };

  friend std::ostream& operator <<( std::ostream& o, const GLLight& q ) {
    return o << "pos ( " << q.position_[0] << ", " << q.position_[1] << ", " << q.position_[2] << ", " << q.position_[3] << ")\namb ( " << q.ambient_[0] << ", " << q.ambient_[1] << ", " << q.ambient_[2] << ", " << q.ambient_[3] << ")\ndif ( " << q.diffuse_[0] << ", " << q.diffuse_[1] << ", " << q.diffuse_[2] << ", " << q.diffuse_[3] << "), spe ( " << q.specular_[0] << ", " << q.specular_[1] << ", " << q.specular_[2] << ", " << q.specular_[3] << ")\n";
  };

private:

  GLint id_;

  bool isOn_;
  
  GLfloat   position_[4];
  GLfloat   ambient_[4];
  GLfloat   diffuse_[4];
  GLfloat   specular_[4];
  GLfloat   const_attenuation_;
  GLfloat   linear_attenuation_;
  GLfloat   quadratic_attenuation_;


};

#endif // _GLLIGHT_HXX

