////////////////////////////////////////////////////////////////////
//
// $Id: Arcball.hxx $
//
// Copyright (c) 2002-2010 by Takashi Kanai. All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _ARCBALL_HXX
#define _ARCBALL_HXX 1

#include "envDep.h"

#include <cstdio>
#include <cmath>
using namespace std;

#include <Quat4.h>
#include <Matrix4.h>
#ifdef VM_INCLUDE_NAMESPACE
using namespace kh_vecmath;
#endif // VM_INCLUDE_NAMESPACE

class Arcball {
  
public:

  Arcball() { init(); };
  ~Arcball(){};

  void init() {
      const Quat4f qOne(0.0, 0.0, 0.0, 1.0);
      center_ = qNow_ = qDown_ = qOne;
      mNow_.set(qNow_);
      mag_ = 1.0;
      magObject_ = 1.0;
      wheelScale_ = .1;
      seezo_ = 0.0;
      scrn_x_ = scrn_y_ = 0;
      offset_.set( .0, .0, .0 );
  };

  Quat4f mouse_on_sphere( const int x, const int y, const int x0, const int y0 ) {

    Quat4f sphere( (x - x0) / radius_, -(y - y0) / radius_, .0, .0 );
    double mag = sphere.x * sphere.x + sphere.y * sphere.y;
    if (mag > 1.0) 
      {
	double scale = 1.0 / std::sqrt(mag);
	sphere.x *= scale;
	sphere.y *= scale;
      }
    else 
      {
	sphere.z = std::sqrt(1.0 - mag);
      }

    return sphere;
  };

  void setDrag( void ) {
    qDrag_.x = vFrom_.y*vTo_.z - vFrom_.z*vTo_.y;
    qDrag_.y = vFrom_.z*vTo_.x - vFrom_.x*vTo_.z;
    qDrag_.z = vFrom_.x*vTo_.y - vFrom_.y*vTo_.x;
    qDrag_.w = vFrom_.x*vTo_.x + vFrom_.y*vTo_.y + vFrom_.z*vTo_.z;
  };

  void setArc( void ) {
    double s = std::sqrt(qDrag_.x*qDrag_.x + qDrag_.y*qDrag_.y);
    if (s == 0.0) {
      vrFrom_.set(0.0, 1.0, 0.0, 0.0);
    } else {
      vrFrom_.set(-qDrag_.y/s, qDrag_.x/s, 0.0, 0.0);
    }
    vrTo_.x = qDrag_.w * vrFrom_.x - qDrag_.z * vrFrom_.y;
    vrTo_.y = qDrag_.w * vrFrom_.y + qDrag_.z * vrFrom_.x;
    vrTo_.z = qDrag_.x * vrFrom_.y - qDrag_.y * vrFrom_.x;
    if (qDrag_.w < 0.0) {
      vrFrom_.set(-vrFrom_.x, -vrFrom_.y, 0.0, 0.0);
    }
  };

  void radius(double r) {radius_ = r;}
  double radius(void) {return radius_;}
  void vFrom(const Quat4f vf) {vFrom_ = vf;}
  void vTo(const Quat4f vt) {vTo_ = vt;}
  void qDown(const Quat4f& q) {qDown_ = q;}
  void mDown(const Matrix4f& m) {mDown_ = m;}
  Quat4f& center(void) {return center_;}
  Quat4f& qNow(void) {return qNow_;}
  Quat4f& qDown(void) {return qDown_;}
  Quat4f& qDrag(void) {return qDrag_;}
  Quat4f& vFrom(void) {return vFrom_;}
  Quat4f& vTo(void) {return vTo_;}
  Quat4f& vrFrom(void) {return vrFrom_;}
  Quat4f& vrTo(void) {return vrTo_;}
  Matrix4f& mNow(void) {return mNow_;}
  Matrix4f& mDown(void) {return mDown_;}

  void setOffset( Point3f& p ) { offset_.set( p ); };
  void setMagObject( float f ) {
    magObject_ = f / 2.0f;
    //seezo_ = magObject_;
  };

  //
  Point3f& offset() { return offset_; };
  float seezo() const { return seezo_; };
  void setSeezo( float z ) { seezo_ = z; };

  //
  void setScrnXY( int x, int y ) {
    scrn_x_ = x; scrn_y_ = y;
  };
  int scrn_x() const { return scrn_x_; };
  int scrn_y() const { return scrn_y_; };

  void setHalfWHL( int w, int h ) {
    halfW_ = w; halfH_ = h;
    radius_ = std::sqrt( (double) halfW_*halfW_ + halfH_*halfH_ );
  };
  int halfW() const { return halfW_; };
  int halfH() const { return halfH_; };
  
  //
  void updateRotate( int dx, int dy ) {
    setDrag();
    qNow().mul(qDrag(), qDown());
    setArc();
    Quat4f qc = qNow();
    qc.conjugate();
    mNow().set(qc);
  };

  void updateMove( int x, int y, int ox, int oy ) {
    double dx = ( x - ox) / 200.0 / mag_ * magObject_;
    double dy = (-y + oy) / 200.0 / mag_ * magObject_;
    Point3f p( mNow().m00 * dx + mNow().m01 * dy,
               mNow().m10 * dx + mNow().m11 * dy,
               mNow().m20 * dx + mNow().m21 * dy );
    offset_.sub(p);
  };

  void updateZoom( int x, int y, int ox, int oy ) {
    seezo_ += (x - ox) * .01 / mag_ * magObject_;
  };

  void updateWheelZoom( int x ) {
    seezo_ -= wheelScale_ * x;
  };

private:

  Quat4f center_;
  double radius_;
  Quat4f qNow_, qDown_, qDrag_;
  Quat4f vFrom_, vTo_, vrFrom_, vrTo_;
  Matrix4f mNow_, mDown_;

  Point3f offset_;
  float mag_;
  float magObject_;
  float wheelScale_;
  float seezo_;

  int scrn_x_, scrn_y_;  // 取得した画面の座標
  int halfW_, halfH_;  // half size of window
  
};

#endif // _ARCBALL_HXX
