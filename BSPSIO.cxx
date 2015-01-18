////////////////////////////////////////////////////////////////////
//
// $Id: BSPSIO.cxx,v 1.4 2005/01/05 14:07:53 kanai Exp $
//
// Copyright (c) 2004-2005 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#include "envDep.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

#include <Point3.h>
#ifdef VM_INCLUDE_NAMESPACE
using namespace kh_vecmath;
#endif // VM_INCLUDE_NAMESPACE

#include "BSPSIO.hxx"

#include "tokenizer.h"

#define NUM_CP 16

bool BSPSIO::inputFromFile( const char* const filename, std::vector<BSPS>& bsps ) 
{
  std::ifstream ifs; ifs.open( filename );
  if ( !ifs.is_open() )
    {
      std::cerr << "Cannot open " << filename << std::endl;
      return false;
    }

  std::string cline;
  getline(ifs, cline, '\n');
  std::istringstream v_isstr( cline );
  int n_patch; v_isstr >> n_patch;
    
  //     cout << n_patch << endl;
    
  std::vector<int> vt;
  std::vector<Point3f> pt;

  for ( int i = 0; i < n_patch; ++i )
    {
      getline(ifs, cline, '\n');
      std::istringstream isstr( cline );

      std::string str; 
      while ( isstr >> str )
	{
	  std::istringstream ai(str); 
	  int id; 
	  ai >> id;
	  vt.push_back( id-1 );
	}

#if 0
      isstr >> str;
      tokenizer tok( str, "," );
      while ( !(tok.empty()) )
	{
	  std::string val = tok.next();
	  int id; 
	  std::istringstream ai(val); 
	  ai >> id;
	  vt.push_back( id-1 );
	  // 	    cout << id << " ";
	}
#endif
      // 	cout << endl;
    }
    
  getline(ifs, cline, '\n');
  std::istringstream p_isstr( cline );
  int n_points; p_isstr >> n_points;

  for ( int i = 0; i < n_points; ++i )
    {
      getline(ifs, cline, '\n');
      std::istringstream isstr( cline );

      std::vector<float> t;
      int j = 0;
	
      std::string str; 
      while ( isstr >> str )
	{
	  float s; 
	  std::istringstream ai(str); 
	  ai >> s;
	  t.push_back( s );
	}
#if 0
      isstr >> str;
      tokenizer tok( str, "," );
      while ( !(tok.empty()) )
	{
	  std::string val = tok.next();
	  float s; 
	  std::istringstream ai(val); 
	  ai >> s;
	  t.push_back( s );
	}
#endif
      pt.push_back( Point3f( t[0], t[1], t[2] ) );

    }

  ifs.close();

  cout << "Patch num: " << n_patch << " Point num: " << n_points << endl;
	
  // conversion from Bezier surface to B-spline surface
  int count = 0;
//   for ( int i = 1; i < 5; ++i ) 
  for ( int i = 0; i < n_patch; i ++ ) 
    {
      BSPS bsp; bsp.setID( count ); count++;
      
      int j;
      std::vector<Point4f> cp;
      for ( j = 0; j < NUM_CP; ++j )
	{
	  //n = 16 * i + j;
 	  //bsp.addCp( pt[ vt[ NUM_CP*i+j ] ] );
	  Point4f p( pt[ vt[ NUM_CP*i+j ] ].x,
		     pt[ vt[ NUM_CP*i+j ] ].y,
		     pt[ vt[ NUM_CP*i+j ] ].z,
		     1.0f );
 	  cp.push_back( p );
	}

#if 0
      bsp.addCp( cp[12] );
      bsp.addCp( cp[13] );
      bsp.addCp( cp[14] );
      bsp.addCp( cp[15] );

      bsp.addCp( cp[8] );
      bsp.addCp( cp[9] );
      bsp.addCp( cp[10] );
      bsp.addCp( cp[11] );

      bsp.addCp( cp[4] );
      bsp.addCp( cp[5] );
      bsp.addCp( cp[6] );
      bsp.addCp( cp[7] );

      bsp.addCp( cp[0] );
      bsp.addCp( cp[1] );
      bsp.addCp( cp[2] );
      bsp.addCp( cp[3] );
#endif

#if 1
      bsp.addCp( cp[0] );
      bsp.addCp( cp[4] );
      bsp.addCp( cp[8] );
      bsp.addCp( cp[12] );

      bsp.addCp( cp[1] );
      bsp.addCp( cp[5] );
      bsp.addCp( cp[9] );
      bsp.addCp( cp[13] );

      bsp.addCp( cp[2] );
      bsp.addCp( cp[6] );
      bsp.addCp( cp[10] );
      bsp.addCp( cp[14] );

      bsp.addCp( cp[3] );
      bsp.addCp( cp[7] );
      bsp.addCp( cp[11] );
      bsp.addCp( cp[15] );
#endif

      for ( j = 0; j < 4; ++j ) bsp.addUkv( .0f );
      for ( j = 0; j < 4; ++j ) bsp.addUkv( 1.0f );
      for ( j = 0; j < 4; ++j ) bsp.addVkv( .0f );
      for ( j = 0; j < 4; ++j ) bsp.addVkv( 1.0f );
      bsp.setNUcp( 4 );
      bsp.setNVcp( 4 );

//       bsp.Print();

      bsps.push_back( bsp );
	
    }

  return true;
}

void BSPSIO::normalizeCp( std::vector<BSPS>& bsps, Point3f& center, float* maxlen ) {
    
  Point4f vmax4, vmin4;

  for ( int j = 0; j < bsps.size(); ++j )
    {
      std::vector<Point4f>& cp = bsps[j].cp();

      for ( int i = 0; i < cp.size(); ++i )
	{
	  if ( (i == 0) && (j == 0) )
	    {
	      vmax4.set( cp[i] ); vmin4.set( cp[i] );
	    }
	  else
	    {
	      if (cp[i].x > vmax4.x) vmax4.x = cp[i].x;
	      if (cp[i].x < vmin4.x) vmin4.x = cp[i].x;
	      if (cp[i].y > vmax4.y) vmax4.y = cp[i].y;
	      if (cp[i].y < vmin4.y) vmin4.y = cp[i].y;
	      if (cp[i].z > vmax4.z) vmax4.z = cp[i].z;
	      if (cp[i].z < vmin4.z) vmin4.z = cp[i].z;
	    }
	}
    }
  Point3f vmax( vmax4.x, vmax4.y, vmax4.z );
  Point3f vmin( vmin4.x, vmin4.y, vmin4.z );

  center = vmax + vmin; center.scale(.5);
  Point3f len = vmax - vmin;
  *maxlen = (std::fabs(len.x) > std::fabs(len.y) )
    ? std::fabs(len.x) : std::fabs(len.y); 
  *maxlen = ( *maxlen > std::fabs(len.z) ) ? *maxlen : std::fabs(len.z);
    
  cout << "Cp are normalized." << endl;
    
  for ( int j = 0; j < bsps.size(); ++j )
    {
      std::vector<Point4f>& cp = bsps[j].cp();

      for ( int i = 0; i < cp.size(); ++i )
	{
	  Point4f p1;
	  p1.x = (cp[i].x - center.x) / *maxlen;
	  p1.y = (cp[i].y - center.y) / *maxlen;
	  p1.z = (cp[i].z - center.z) / *maxlen;
	  p1.w = cp[i].w;
	  cp[i].set( p1 );
	}
    }
}
