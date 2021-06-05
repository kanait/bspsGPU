////////////////////////////////////////////////////////////////////
//
// $Id: GLBSPS.hxx 2021/06/05 12:18:53 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _GLBSPS_HXX
#define _GlBSPS_HXX 1

#include "GLMesh.hxx"

class GLBSPS : public GLMesh {

public:

  GLBSPS() : bsps_(NULL) {};
  GLBSPS( BSPS& bsps ) { setBSPS( bsps ); };
  ~GLBSPS() {};

  void setBSPS( BSPS& bsps ) { bsps_ = &bsps; };
  BSPS& bsps() { return *bsps_; };

  void drawWireframe() {

    ::glColor3fv( wireColor() );

    ::glEnable( GL_LINE_SMOOTH );
    ::glLineWidth( wireSize() );

    int div = 50;

    float u, v;
    
    v = bsps().paramV( 0, div );
    ::glBegin( GL_LINE_STRIP );
    for ( int i = 0; i <= div; ++i )
      {
	u = bsps().paramU( i, div );
	Point3f p; Vector3f n;
	bsps().eval3( u, v, p, n );
	::glVertex3f( p.x, p.y, p.z );
      }
    ::glEnd();

    v = bsps().paramV( div, div );
    ::glBegin( GL_LINE_STRIP );
    for ( int i = 0; i <= div; ++i )
      {
	u = bsps().paramU( i, div );
	Point3f p; Vector3f n;
	bsps().eval3( u, v, p, n );
	::glVertex3f( p.x, p.y, p.z );
      }
    ::glEnd();

    u = bsps().paramU( 0, div );
    ::glBegin( GL_LINE_STRIP );
    for ( int i = 0; i <= div; ++i )
      {
	v = bsps().paramV( i, div );
	Point3f p; Vector3f n;
	bsps().eval3( u, v, p, n );
	::glVertex3f( p.x, p.y, p.z );
      }
    ::glEnd();

    u = bsps().paramU( div, div );
    ::glBegin( GL_LINE_STRIP );
    for ( int i = 0; i <= div; ++i )
      {
	v = bsps().paramV( i, div );
	Point3f p; Vector3f n;
	bsps().eval3( u, v, p, n );
	::glVertex3f( p.x, p.y, p.z );
      }
    ::glEnd();

  };

private:

  BSPS* bsps_; 
  
};
#endif // _GLBSPS_HXX

