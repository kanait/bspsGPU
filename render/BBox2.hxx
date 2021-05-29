////////////////////////////////////////////////////////////////////
//
// $Id: BBox2.hxx,v 1.1.1.1 2005/04/01 17:12:52 kanai Exp $
//
// Copyright (c) 2002 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _BBOX2_HXX
#define _BBOX2_HXX 1

#include <cmath>

#include <Point2.h>
#ifdef VM_INCLUDE_NAMESPACE
using namespace kh_vecmath;
#endif // VM_INCLUDE_NAMESPACE

class BBox2 {

  Point2i start;
  Point2i end;

public:

  BBox2() {};
  ~BBox2() {};

  void clear() { start.set(0,0); end.set(0,0); };
  void setStart( int x, int y ) { start.set( x, y ); };
  void setEnd( int x, int y ) { end.set( x, y ); };

  int& sx() { return start.x; };
  int& sy() { return start.y; };
  int& ex() { return end.x; };
  int& ey() { return end.y; };

  bool isRectangle() {
    if ( (std::abs(start.x - end.x) > 4) && (std::abs(start.y - end.y) > 4) )
      return true;
    return false;
  };
};
  
#endif // _BBOX2_HXX
