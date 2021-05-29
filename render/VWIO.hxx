////////////////////////////////////////////////////////////////////
//
// $Id: VWIO.hxx,v 1.1.1.1 2005/04/01 17:12:52 kanai Exp $
//
// Copyright (c) 2002 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _VWIO_HXX
#define _VWIO_HXX 1

#include "envDep.h"

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "Point3.h"
#include "Quat4.h"
#ifdef VM_INCLUDE_NAMESPACE
using namespace kh_vecmath;
#endif // VM_INCLUDE_NAMESPACE

#include "Arcball.hxx"

class VWIO {

public:

  VWIO() {};
  ~VWIO() {};

  bool inputFromFile( const char* const filename, Arcball& manip ) {
    std::ifstream ifs( filename );
    if ( !ifs )
      {
	std::cerr << "Cannot open " << filename << std::endl;
	return false;
      }

    std::string cline;
    char buf[BUFSIZ], dummy[BUFSIZ];

    // comment
    ifs.getline(buf, BUFSIZ, '\n');

    // seezo
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    float seezo;
    sscanf( cline.c_str(), "%s%f", dummy, &seezo );
    manip.setSeezo( seezo );

    // eye
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    Point3f o;
    sscanf( cline.c_str(), "%s%f%f%f", dummy, &(o.x), &(o.y), &(o.z) );
    manip.offset().set( o );

    // center
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    char val[BUFSIZ];
    sscanf( cline.c_str(), "%s%s", dummy, val );
    manip.radius( atof(val) );

    Quat4f dq;
  
    // center
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(dq.x), &(dq.y), &(dq.z), &(dq.w) );
    manip.center().set( dq );
  
    // qNow
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(dq.x), &(dq.y), &(dq.z), &(dq.w) );
    manip.qNow().set( dq );
  
    // qDown
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(dq.x), &(dq.y), &(dq.z), &(dq.w) );
    manip.qDown().set( dq );
  
    // qDrag
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(dq.x), &(dq.y), &(dq.z), &(dq.w) );
    manip.qDrag().set( dq );
  
    // vFrom
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(dq.x), &(dq.y), &(dq.z), &(dq.w) );
    manip.vFrom().set( dq );
  
    // vTo
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(dq.x), &(dq.y), &(dq.z), &(dq.w) );
    manip.vTo().set( dq );
  
    // vrFrom
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(dq.x), &(dq.y), &(dq.z), &(dq.w) );
    manip.vrFrom().set( dq );
  
    // vrTo
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(dq.x), &(dq.y), &(dq.z), &(dq.w) );
    manip.vrTo().set( dq );
  
    // mNow
    ifs.getline(buf, BUFSIZ, '\n');
    float m[4][4];
    unsigned int i;
    for ( i = 0; i < 4; ++i )
      {
	ifs.getline(buf, BUFSIZ, '\n');
	cline.assign( buf );
	sscanf( cline.c_str(), "%f%f%f%f", &(m[i][0]), &(m[i][1]), &(m[i][2]), &(m[i][3]) );
      }
    manip.mNow().set( m[0][0], m[0][1], m[0][2], m[0][3],
		      m[1][0], m[1][1], m[1][2], m[1][3],
		      m[2][0], m[2][1], m[2][2], m[2][3],
		      m[3][0], m[3][1], m[3][2], m[3][3] );

    // mDown
    ifs.getline(buf, BUFSIZ, '\n');
    for ( i = 0; i < 4; ++i )
      {
	ifs.getline(buf, BUFSIZ, '\n');
	cline.assign( buf );
	sscanf( cline.c_str(), "%f%f%f%f",
		&(m[i][0]), &(m[i][1]), &(m[i][2]), &(m[i][3]) );
      }
  
    manip.mDown().set( m[0][0], m[0][1], m[0][2], m[0][3],
		       m[1][0], m[1][1], m[1][2], m[1][3],
		       m[2][0], m[2][1], m[2][2], m[2][3],
		       m[3][0], m[3][1], m[3][2], m[3][3] );
  
    ifs.close();

    return true;
  };

  bool outputToFile( const char* const filename, Arcball& manip ) {
    std::ofstream ofs( filename );
    if ( !ofs )
      {
	std::cerr << "Cannot open " << filename << std::endl;
	return false;
      }

  
  
    ofs << "#VW Version 2.0" << std::endl;

    ofs << "seezo " << manip.seezo() << std::endl;
  
    Point3f o = manip.offset();
    ofs << "eye " << o.x << " " << o.y << " " << o.z << std::endl;

    ofs << "radius " << manip.radius() << std::endl;

    ofs << "center ";
    ofs << manip.center().x << " " << manip.center().y << " ";
    ofs << manip.center().z << " " << manip.center().w << std::endl;
  
    ofs << "qNow ";
    ofs << manip.qNow().x << " " << manip.qNow().y << " ";
    ofs << manip.qNow().z << " " << manip.qNow().w << std::endl;
  
    ofs << "qDown ";
    ofs << manip.qDown().x << " " << manip.qDown().y << " ";
    ofs << manip.qDown().z << " " << manip.qDown().w << std::endl;
  
    ofs << "qDrag ";
    ofs << manip.qDrag().x << " " << manip.qDrag().y << " ";
    ofs << manip.qDrag().z << " " << manip.qDrag().w << std::endl;
  
    ofs << "vFrom ";
    ofs << manip.vFrom().x << " " << manip.vFrom().y << " ";
    ofs << manip.vFrom().z << " " << manip.vFrom().w << std::endl;
  
    ofs << "vTo ";
    ofs << manip.vTo().x << " " << manip.vTo().y << " ";
    ofs << manip.vTo().z << " " << manip.vTo().w << std::endl;
  
    ofs << "vrFrom ";
    ofs << manip.vrFrom().x << " " << manip.vrFrom().y << " ";
    ofs << manip.vrFrom().z << " " << manip.vrFrom().w << std::endl;
  
    ofs << "vrTo ";
    ofs << manip.vrTo().x << " " << manip.vrTo().y << " ";
    ofs << manip.vrTo().z << " " << manip.vrTo().w << std::endl;
  
    ofs << "mNow" << std::endl;
    ofs << manip.mNow().m00 << " " << manip.mNow().m01 << " ";
    ofs << manip.mNow().m02 << " " << manip.mNow().m03 << std::endl;
    ofs << manip.mNow().m10 << " " << manip.mNow().m11 << " ";
    ofs << manip.mNow().m12 << " " << manip.mNow().m13 << std::endl;
    ofs << manip.mNow().m20 << " " << manip.mNow().m21 << " ";
    ofs << manip.mNow().m22 << " " << manip.mNow().m23 << std::endl;
    ofs << manip.mNow().m30 << " " << manip.mNow().m31 << " ";
    ofs << manip.mNow().m32 << " " << manip.mNow().m33 << std::endl;
  
    ofs << "mDown" << std::endl;
    ofs << manip.mDown().m00 << " " << manip.mDown().m01 << " ";
    ofs << manip.mDown().m02 << " " << manip.mDown().m03 << std::endl;
    ofs << manip.mDown().m10 << " " << manip.mDown().m11 << " ";
    ofs << manip.mDown().m12 << " " << manip.mDown().m13 << std::endl;
    ofs << manip.mDown().m20 << " " << manip.mDown().m21 << " ";
    ofs << manip.mDown().m22 << " " << manip.mDown().m23 << std::endl;
    ofs << manip.mDown().m30 << " " << manip.mDown().m31 << " ";
    ofs << manip.mDown().m32 << " " << manip.mDown().m33 << std::endl;
  
    ofs.close();

    return true;
  };

};

#endif // _VWIO_HXX

