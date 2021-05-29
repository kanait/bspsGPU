////////////////////////////////////////////////////////////////////
//
// $Id: $
//
// Copyright (c) 2005-2010 by Takashi Kanai. All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _OFFRIO_HXX
#define _OFFRIO_HXX 1

#include "envDep.h"

#if defined(_WINDOWS)
#include "stdafx.h"
#endif

#include "mydef.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

#include "timer.hxx"

#include "MeshR.hxx"
#include "RIO.hxx"

#include "gzstream.h"

class OFFRIO : public RIO {

public:

  OFFRIO() : RIO() {};
  OFFRIO( MeshR& mesh ) : RIO( mesh ) {};
  ~OFFRIO() {};
  
  bool inputGZFromFile( const char* const filename ) {
    //
    // file open
    //
    igzstream ifs; ifs.open( filename );
    if ( !ifs.is_open() )
      {
        std::cerr << "Cannot open " << filename << std::endl;
        return false;
      }

    Timer t;
    double time0 = t.get_seconds();

    std::string cline;
    getline(ifs, cline, '\n'); // OFF
    if ( cline.find("OFF") == string::npos )
      //if ( cline.compare("OFF") && cline.compare("COFF") )
      {
        std::cerr << "This is not OFF file. " << filename << std::endl;
        return false;
      }

    // count #vertices, #faces
    getline(ifs, cline, '\n'); // OFF
    std::istringstream isstr( cline );
    int v_count, f_count;
    isstr >> v_count;
    if ( v_count )  mesh().reservePoints( v_count );
    isstr >> f_count;
    if ( f_count )  mesh().reserveIndices( f_count );

    // read vertices
    int v_id = 0;
    for ( int i = 0; i < v_count; ++i )
      {
        getline(ifs, cline, '\n'); 
        char xc[BUFSIZ], yc[BUFSIZ], zc[BUFSIZ];
        sscanf( cline.c_str(), "%s%s%s", &xc, &yc, &zc );
        float x = atof( xc );
        float y = atof( yc );
        float z = atof( zc );

        mesh().setPoint( v_id, x, y, z );
        v_id += nXYZ;
      }

    // read faces
    int f_id = 0;
    for ( int i = 0; i < f_count; ++i )
      {
        getline(ifs, cline, '\n');

        char dummy[BUFSIZ];
        char xc[BUFSIZ], yc[BUFSIZ], zc[BUFSIZ];
        sscanf( cline.c_str(), "%s%s%s%s", &dummy, &xc, &yc, &zc );
        unsigned int id0 = atoi( xc );
        unsigned int id1 = atoi( yc );
        unsigned int id2 = atoi( zc );
        mesh().setIndex( f_id, id0 ); ++f_id;
        mesh().setIndex( f_id, id1 ); ++f_id;
        mesh().setIndex( f_id, id2 ); ++f_id;
      }

    double time1 = t.get_seconds();
    std::cout << "done. ellapsed time: " << time1 - time0 << " sec. " << std::endl;

    ifs.close();

    mesh().printInfo();

    return true;
  };
  
#if 0
  bool inputFromFile( std::string& filestr ) {

    std::istringstream ifs( filestr );
    if ( ifs.fail() )
      {
        cout << "fail to load file." << endl;
        return false;
      }

    Timer t;
    double time0 = t.get_seconds();

    std::string cline;
    getline(ifs, cline, '\n'); // OFF
    //cout << cline.compare("OFF\n") << endl;
#if 1
    if ( cline.find("OFF") == string::npos )
      {
        std::cerr << "This is not OFF file. " << std::endl;
        return false;
      }
#endif

    // count #vertices, #faces
    getline(ifs, cline, '\n'); // OFF
    std::istringstream isstr( cline );
    int v_count, f_count;
    isstr >> v_count;
    if ( v_count )  mesh().reservePoints( v_count );
    isstr >> f_count;
    if ( f_count )  mesh().reserveIndices( f_count );

    // read vertices
    int v_id = 0;
    for ( int i = 0; i < v_count; ++i )
      {
        getline(ifs, cline, '\n'); 
        char xc[BUFSIZ], yc[BUFSIZ], zc[BUFSIZ];
        sscanf( cline.c_str(), "%s%s%s", &xc, &yc, &zc );
        float x = atof( xc );
        float y = atof( yc );
        float z = atof( zc );

        mesh().setPoint( v_id, x, y, z );
        v_id += nXYZ;
      }

    // read faces
    int f_id = 0;
    for ( int i = 0; i < f_count; ++i )
      {
        getline(ifs, cline, '\n');

        char dummy[BUFSIZ];
        char xc[BUFSIZ], yc[BUFSIZ], zc[BUFSIZ];
        sscanf( cline.c_str(), "%s%s%s%s", &dummy, &xc, &yc, &zc );
        unsigned int id0 = atoi( xc );
        unsigned int id1 = atoi( yc );
        unsigned int id2 = atoi( zc );
        mesh().setIndex( f_id, id0 ); ++f_id;
        mesh().setIndex( f_id, id1 ); ++f_id;
        mesh().setIndex( f_id, id2 ); ++f_id;
      }

    double time1 = t.get_seconds();
    std::cout << "done. ellapsed time: " << time1 - time0 << " sec. " << std::endl;

    //ifs.close();

    mesh().printInfo();

    return true;
  };
#endif  

  bool inputFromFile( const char* const filename ) {
    //
    // file open
    //
    std::ifstream ifs; ifs.open( filename );
    if ( !ifs.is_open() )
      {
        std::cerr << "Cannot open " << filename << std::endl;
        return false;
      }

    Timer t;
    double time0 = t.get_seconds();

    std::string cline;
    getline(ifs, cline, '\n'); // OFF
    if ( cline.find("OFF") == string::npos )
      //if ( cline.compare("OFF") && cline.compare("COFF") )
      {
        std::cerr << "This is not OFF file. " << filename << std::endl;
        return false;
      }

    // count #vertices, #faces
    getline(ifs, cline, '\n'); // OFF
    std::istringstream isstr( cline );
    int v_count, f_count;
    isstr >> v_count;
    if ( v_count )  mesh().reservePoints( v_count );
    isstr >> f_count;
    if ( f_count )  mesh().reserveIndices( f_count );

    // read vertices
    int v_id = 0;
    for ( int i = 0; i < v_count; ++i )
      {
        getline(ifs, cline, '\n'); 
        char xc[BUFSIZ], yc[BUFSIZ], zc[BUFSIZ];
        sscanf( cline.c_str(), "%s%s%s", &xc, &yc, &zc );
        float x = atof( xc );
        float y = atof( yc );
        float z = atof( zc );

        mesh().setPoint( v_id, x, y, z );
        v_id += nXYZ;
      }

    // read faces
    int f_id = 0;
    for ( int i = 0; i < f_count; ++i )
      {
        getline(ifs, cline, '\n');

        char dummy[BUFSIZ];
        char xc[BUFSIZ], yc[BUFSIZ], zc[BUFSIZ];
        sscanf( cline.c_str(), "%s%s%s%s", &dummy, &xc, &yc, &zc );
        unsigned int id0 = atoi( xc );
        unsigned int id1 = atoi( yc );
        unsigned int id2 = atoi( zc );
        mesh().setIndex( f_id, id0 ); ++f_id;
        mesh().setIndex( f_id, id1 ); ++f_id;
        mesh().setIndex( f_id, id2 ); ++f_id;
      }

    double time1 = t.get_seconds();
    std::cout << "done. ellapsed time: " << time1 - time0 << " sec. " << std::endl;

    ifs.close();

    mesh().printInfo();

    return true;
  };

  bool outputToFile( const char* const filename ) {
    std::ofstream ofs( filename ); if ( !ofs ) return false;

    std::cout << "Output to file: " << filename << endl;

    mesh().printInfo();

    // header
  
    ofs << "OFF" << std::endl;

    int vn = (int) ((float) mesh().points().size() / (float) nXYZ);
    int fn = (int) ((float) mesh().indices().size() / (float) TRIANGLE);
    ofs << vn << " " << fn << " 0 " << std::endl;

    if ( vn )
      {
        std::vector<float>& points = mesh().points();
        for ( int i = 0; i < vn; ++i )
          {
            ofs << points[ nXYZ * i ] << " "
                << points[ nXYZ * i + 1 ]  << " "
                << points[ nXYZ * i + 2 ]  << std::endl;
          }
      }

    if ( fn )
      {
        std::vector<unsigned int>& indices = mesh().indices();
        for ( int i = 0; i < fn; ++i )
          {
            ofs << "3 ";
            for ( int j = 0; j < TRIANGLE; ++j )
              {
                unsigned int id = indices[ TRIANGLE * i + j ];
                ofs << id << " ";
              }
            ofs << std::endl;
          }
      }

    ofs.close();

    return true;
  };

};

#endif // _OFFRIO_H
