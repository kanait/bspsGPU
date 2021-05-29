////////////////////////////////////////////////////////////////////
//
// $Id: $
//
// Copyright (c) 2010 by Takashi Kanai. All rights reserved.
//
////////////////////////////////////////////////////////////////////

#ifndef _SELMESH_HXX
#define _SELMESH_HXX 1

#include <GL/gl.h>
#include <GL/glu.h>

#include "GLPanel.hxx"
#include "BBox2.hxx"

#define NUM_PER_SELBUF 5

class SelMesh {

public:

  SelMesh() : numSelected_(0) {};
  SelMesh( GLPanel& glpane ) : numSelected_(0) {
    setGLPanel( glpane );
  };
  virtual ~SelMesh(){};

  // pick nodes into a selected box
  void init( GLPanel& glpane, BBox2& bbox ) {
    setGLPanel( glpane );
    setBBox2( bbox );
  };

  // pick one node
  void init( GLPanel& glpane ) {
    setGLPanel( glpane );
  }

  void setCoord( int x, int y ) { x_ = x; y_ = y; };

  void setRectMode( bool f ) { rect_mode_ = f; };
  bool rectMode() const { return rect_mode_; };

  GLPanel& glpane() { return *glpane_; };
  void setGLPanel( GLPanel& glpane ) { glpane_ = &glpane; };

  BBox2& bbox() { return *sel_rect_; };
  void setBBox2( BBox2& bbox ) { sel_rect_ = &bbox; };

  void setSelectID( int size ) {
    maxSelect_ = size * NUM_PER_SELBUF;
    maxID_ = size;
  };

  unsigned int numSelected() const { return numSelected_; };
  void countNumSelected() { ++numSelected_; };
  void clearNumSelected() { numSelected_=0; };

  void setMode( int m ) { mode_ = m; };
  int mode() const { return mode_; };

  GLuint* initialize() {
    GLuint* selectBuf = new GLuint[ maxSelect_ ];
    ::glSelectBuffer( maxSelect_, &selectBuf[0] );
    ::glRenderMode( GL_SELECT );
    ::glInitNames();
    ::glPushName(~0);
    return selectBuf;
  };
  void setProjectionView() {
    ::glViewport( 0, 0, glpane_->w(), glpane_->h() );
    GLint vp[4];
    ::glGetIntegerv( GL_VIEWPORT, vp );

    ::glMatrixMode( GL_PROJECTION );
    ::glLoadIdentity();

    if ( rectMode() == true )
      {
        ::gluPickMatrix( (sel_rect_->sx()+sel_rect_->ex())/2.0,
                         (sel_rect_->sy()+sel_rect_->ey())/2.0,
                         std::fabs( (double) (sel_rect_->sx() - sel_rect_->ex()) ),
                         std::fabs( (double) (sel_rect_->sy() - sel_rect_->ey()) ),
                         vp );
      }
    else
      {
        ::gluPickMatrix( x_-2, y_-2, 5, 5, vp );
      }
    glpane_->setPerspective();
  };

protected:

  GLPanel* glpane_;

  // true if picking nodes into a selected box
  bool rect_mode_;
  BBox2* sel_rect_;

  // screen coordinates (for picking one node)
  int x_, y_;

  int mode_;
  int maxSelect_;
  // max id of selected nodes
  int maxID_;

  int numSelected_;

};

#endif // _SELMESH_HXX

