////////////////////////////////////////////////////////////////////
//
// $Id: $
//
// Copyright (c) 2006 by Takashi Kanai. All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#include "envDep.h"
#include "mydefine.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include <Point3.h>
#ifdef VM_INCLUDE_NAMESPACE
using namespace kh_vecmath;
#endif // VM_INCLUDE_NAMESPACE

#include "POLIO.hxx"

bool POLIO::inputFromFile( const char* const filename )
{
  std::ifstream ifs; ifs.open( filename );
  if ( !ifs.is_open() )
    {
      std::cerr << "Cannot open " << filename << std::endl;
      return false;
    }

  int real_face_id = -1;
  int face_id_org = -1;
  
  unsigned int c = 0;
  std::string cline;
  while ( getline(ifs, cline, '\n') )
    {
      std::istringstream isstr( cline );
      
      std::string dummy; isstr >> dummy;
      
      int face_id; isstr >> face_id;
      if ( face_id != face_id_org ) 
	{
	  real_face_id++;
	}

      for ( int i = 0; i < TRIANGLE; ++i )
	{
	  getline(ifs, cline, '\n');
	  std::istringstream isstr0( cline );

	  isstr0 >> dummy; isstr0 >> dummy;

	  std::string xstr, ystr, zstr;
	  float x, y, z;

	  // points
	  isstr0 >> xstr; xstr.erase( xstr.size()-1 );
	  isstr0 >> ystr; ystr.erase( ystr.size()-1 );
	  isstr0 >> zstr; 
	  x = atof( xstr.c_str() );
	  y = atof( ystr.c_str() );
	  z = atof( zstr.c_str() );
	  mesh().addPoint( x, y, z );
// 	  cout << "points: " << x << " " << y << " " << z << endl;

	  isstr0 >> dummy; isstr0 >> dummy; isstr0 >> dummy;

	  // normals
	  isstr0 >> xstr; xstr.erase( xstr.size()-1 );
	  isstr0 >> ystr; ystr.erase( ystr.size()-1 );
	  isstr0 >> zstr; 
	  x = atof( xstr.c_str() );
	  y = atof( ystr.c_str() );
	  z = atof( zstr.c_str() );
	  mesh().addNormal( x, y, z );
// 	  cout << "normals: " << x << " " << y << " " << z << endl;

	  isstr0 >> dummy; isstr0 >> dummy; isstr0 >> dummy;
	  
	  // texcoords
	  isstr0 >> xstr; xstr.erase( xstr.size()-1 );
	  isstr0 >> ystr;
	  x = atof( xstr.c_str() );
	  y = atof( ystr.c_str() );
	  float a = (float) real_face_id / FID_DIV;
 	  mesh().addTexcoord( x, y, a );
// 	  cout << "texcoords: " << x << " " << y << endl;

	  mesh().addIndex( c ); 
	  mesh().addNIndex( c ); 
	  ++c;
	}

      face_id_org = face_id;
      
    }
  
  ifs.close();

  mesh().printInfo();

  return true;
}


