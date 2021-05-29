////////////////////////////////////////////////////////////////////
//
// $Id: ColorConv.hxx,v 1.1.1.1 2005/04/01 17:12:52 kanai Exp $
//
// Copyright (c) 2002-2003 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _COLORCONV_HXX
#define _COLORCONV_HXX 1

#include <cmath>
using namespace std;

#include "Point3.h"
#ifdef VM_INCLUDE_NAMESPACE
using namespace kh_vecmath;
#endif // VM_INCLUDE_NAMESPACE

class ColorConv {

public:

  ColorConv(){};
  ~ColorConv(){};

  static void hsvtorgb( Point3f& hsv, Point3f& rgb ) {
    
    if ( hsv.y >= 1.0f ) hsv.y = 1.0f;
    else if ( hsv.y <= 0.0f ) hsv.y = 0.0f;

    if ( hsv.z >= 1.0 ) hsv.z = 1.0f;
    else if ( hsv.z <= 0.0f ) hsv.z = 0.0f;
    
    if ( hsv.y == 0.0 )
      {
        rgb.set( hsv.z, hsv.z, hsv.z );
      }
    else
      {
        float hh = hsv.x * 3.0 / M_PI;
        int i = (int) hh;
        if ( i > 5 )
          {
            hsv.x -= 6.0;
            i = 0;
        }
        float f = hsv.x - (double) i;
        float p = hsv.z * (1.0 - hsv.y);
        float q = hsv.z * (1.0 - hsv.y * f);
        float t = hsv.z * (1.0 - hsv.y * (1.0 - f));
        
        if ( i == 0 )
          {
            rgb.set( hsv.z, t, p );
          }
        else if ( i == 1 )
          {
            rgb.set( q, hsv.z, p );
          }
        else if ( i == 2 )
          {
            rgb.set( p, hsv.z, t );
          }
        else if ( i == 3 )
          {
            rgb.set( p, q, hsv.z );
          }
        else if ( i == 4 )
          {
            rgb.set( t, p, hsv.z );
          }
        else
          {
            rgb.set( hsv.z, p, q );
          }
      }
  };

  static void rgbtohsv( Point3f& rgb, Point3f& hsv ) {

    rgb.clamp( 0.0f, 1.0f );
    
    float maxrgb = rgb.x;
    if ( maxrgb < rgb.y ) maxrgb = rgb.y;
    if ( maxrgb < rgb.z ) maxrgb = rgb.z;
    float minrgb = rgb.x;
    if ( minrgb > rgb.y ) minrgb = rgb.y;
    if ( minrgb > rgb.z ) minrgb = rgb.z;
    
    hsv.z = maxrgb;
    
    if (maxrgb != 0.0)
      {
        hsv.y = (maxrgb - minrgb) / maxrgb;
      }
    else
      hsv.y = 0.0f;
    
    if (hsv.y == 0.0f)
      hsv.x = 0.0f;
    else
      {
        float delta = maxrgb - minrgb;
        if ( rgb.x == maxrgb )
          hsv.x = (rgb.y - rgb.z) / delta;
        else if ( rgb.y == maxrgb )
          hsv.x = 2.0f + (rgb.z - rgb.x) / delta;
        else if ( rgb.z == maxrgb )
          hsv.x = 4.0f + (rgb.x - rgb.y) / delta;
        hsv.x *= M_PI / 3.0f;
        if (hsv.x < 0.0f)
          hsv.x += 2.0f * M_PI;
      }
  };

};

#endif
