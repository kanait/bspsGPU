////////////////////////////////////////////////////////////////////
//
// $Id: $
//
// Copyright (c) 2005-2010 by Takashi Kanai. All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _OBJRIO_HXX
#define _OBJRIO_HXX 1

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
#include "strutil.h"
#include "tokenizer.h"

#include "MeshR.hxx"
#include "RIO.hxx"

class OBJRIO : public RIO {

public:

  OBJRIO() : RIO() {};
  OBJRIO( MeshR& mesh ) : RIO( mesh ) {};
  ~OBJRIO() {};
  
  bool inputFromFile( const char* const filename ) {
    //
    // file open
    //

    // 1st try
    std::ifstream ifs0; ifs0.open( filename );
    if ( !ifs0.is_open() )
      {
        std::cerr << "Cannot open " << filename << std::endl;
        return false;
      }

    std::cout << "Input from file: " << filename << endl;


    // count elements
    int v_count = 0;  // vertex
    int n_count = 0;  // normal
    int r_count = 0;  // texcoord
    int c_count = 0;  // color
    int cf_count = 0;  // color id
    int f_count = 0;  // face indices

    std::cout << "count elements ..." << std::endl;

    std::string cline;
    StrUtil strutil;
    while ( getline(ifs0, cline, '\n') )
      {
	std::string fw;
	strutil.first_word( cline, fw );

	// comment
	if ( mycomment(cline[0]) ) continue;

	else if ( !fw.compare("v") ) ++v_count;
	else if ( !fw.compare("n") || !fw.compare("vn") ) ++n_count;
	else if ( !fw.compare("r") ) 
	  {
	    if ( !r_count )
	      {
		int wc = strutil.word_count( cline );
		if ( wc == 3 ) mesh().setNTex( 2 );
		else mesh().setNTex( 3 );
	      }
	    ++r_count;
	  }
	else if ( !fw.compare("c") ) ++c_count;
	else if ( !fw.compare("cf") ) ++cf_count;
	else if ( !fw.compare("f") ) ++f_count;
      }

    ifs0.close();

    std::cout << "done." << endl;

    std::cout << "meshR:";
    if ( v_count )  std::cout << " v "  << v_count;
    if ( n_count )  std::cout << " n "  << n_count;
    if ( r_count )  std::cout << " r "  << r_count;
    if ( c_count )  std::cout << " c "  << c_count;
    if ( cf_count ) std::cout << " cf " << cf_count;
    if ( f_count )  std::cout << " f "  << f_count;
    std::cout << std::endl;

    std::cout << "read data .. " << std::endl;

    if ( v_count )  mesh().reservePoints(    v_count );
    if ( n_count )  mesh().reserveNormals(   n_count );
    if ( r_count )  mesh().reserveTexcoords( r_count, mesh().n_tex() );
    if ( c_count )  mesh().reserveColors(    c_count );
    if ( cf_count ) mesh().reserveColorIds(  cf_count );
    if ( f_count )  mesh().reserveIndices(   f_count );
    if ( n_count && f_count )  mesh().reserveNIndices(   f_count );

  
    // 2nd try
    std::ifstream ifs; ifs.open( filename );
    if ( !ifs.is_open() )
      {
	std::cerr << "Cannot open " << filename << std::endl;
	return false;
      }

    //
    // parse smf
    //
    int v_id = 0;
    int n_id = 0;
    int r_id = 0;
    int c_id = 0;
    int cf_id = 0;
    int f_id = 0;
    Timer t;
    double time0 = t.get_seconds();
    while ( getline(ifs, cline, '\n') )
      {
	std::string fw;
	strutil.first_word( cline, fw );
      
	// comment
	if ( mycomment(cline[0]) ) continue;

	// read vertices
	else if ( !fw.compare("v") )
	  {
#if 0
	    std::istringstream isstr( cline );

	    // "v"
	    std::string str; isstr >> str;

	    float x, y, z;
	    isstr >> x; isstr >> y; isstr >> z;
#endif

#if 1
	    char dummy[BUFSIZ];
	    char xc[BUFSIZ], yc[BUFSIZ], zc[BUFSIZ];
	    sscanf( cline.c_str(), "%s%s%s%s", &dummy, &xc, &yc, &zc );
	    float x = atof( xc );
	    float y = atof( yc );
	    float z = atof( zc );
#endif

            //            cout << v_id/3 << " " << x << " " << y << " " << z << endl;
	    mesh().setPoint( v_id, x, y, z );
	    v_id += nXYZ;
	  }

	// read normals
	else if ( !fw.compare("n") || !fw.compare("vn") )
	  {
#if 0
	    std::istringstream isstr( cline );
	  
	    // "n"
	    std::string str; isstr >> str;

	    float x, y, z;
	    isstr >> x; isstr >> y; isstr >> z;
#endif

#if 1
	    char dummy[BUFSIZ];
	    char xc[BUFSIZ], yc[BUFSIZ], zc[BUFSIZ];
	    sscanf( cline.c_str(), "%s%s%s%s", &dummy, &xc, &yc, &zc );
	    float x = atof( xc );
	    float y = atof( yc );
	    float z = atof( zc );
            //            cout << x << " " << y << " " << z << endl;
#endif

	    mesh().setNormal( n_id, x, y, z );
	    n_id += nXYZ;
	  }

	// read texcoords
	else if ( !fw.compare("r") )
	  {
	    char dummy[BUFSIZ];
	    char xc[BUFSIZ], yc[BUFSIZ], zc[BUFSIZ];
	    float x, y, z;
	    if ( mesh().n_tex() == 2 )
	      {
		sscanf( cline.c_str(), "%s%s%s", &dummy, &xc, &yc );
		x = atof( xc );
		y = atof( yc );
		mesh().setTexcoord( r_id, x, y );
	      }
	    else if ( mesh().n_tex() == 3 )
	      {
		sscanf( cline.c_str(), "%s%s%s%s", &dummy, &xc, &yc, &zc );
		x = atof( xc );
		y = atof( yc );
		z = atof( zc );
		mesh().setTexcoord( r_id, x, y, z );
	      }
	    r_id += mesh().n_tex();
	  }

	// binding types
	else if ( !fw.compare("bind") )
	  {
	    std::string s;
	    strutil.nth_word( cline, 3, s );
	    if ( !s.compare("vertex") )
	      mesh().setColorAssigned( ASSIGN_VERTEX );
	    else if ( !s.compare("face") )
	      mesh().setColorAssigned( ASSIGN_FACE );
	  }

	// colors
	else if ( !fw.compare("c") )
	  {
	    char dummy[BUFSIZ];
	    char xc[BUFSIZ], yc[BUFSIZ], zc[BUFSIZ];
	    sscanf( cline.c_str(), "%s%s%s%s", &dummy, &xc, &yc, &zc );
	    float x = atof( xc );
	    float y = atof( yc );
	    float z = atof( zc );

	    mesh().setColor( c_id, x, y, z );
	    c_id += nXYZ;
	  }

	// face color ids
	else if ( !fw.compare("cf") )
	  {
	    char dummy[BUFSIZ];
	    char ids[BUFSIZ];
	    sscanf( cline.c_str(), "%s%s", &dummy, &ids );
	    unsigned int id = atoi( ids );

	    mesh().setColorId( cf_id, id );
	    ++ cf_id;
	  }
      
	// read faces
	else if ( !fw.compare("f") )
	  {
	    if ( cline.find("/") != std::string::npos )
	      {
		std::istringstream isstr( cline );

		// "f"
		std::string str; isstr >> str;

		int j = 0;
		while ( isstr >> str )
		  {
		    if ( j >= 3 ) break;
		    tokenizer tok( str, "//" );
		  
		    // vertex
		    std::string val = tok.next();

		    int id0;
		    unsigned int id;
		    std::istringstream ai(val); ai >> id0;
		    id = id0 - 1;
		    mesh().setIndex( f_id, id );
		    // 		  cout << id << " ";
	      
		    // normal
		    if ( n_count )
		      {
			val = tok.next();
			int id1;
			std::istringstream ai(val); ai >> id1;
			id = id1 - 1;
			mesh().setNIndex( f_id, id );
			// 		      cout << id << " ";
		      }
	      
		    ++f_id;
		    ++j;
		  }
		// 	      cout << endl;
	      }
	    else
	      {
		char dummy[BUFSIZ];
		char xc[BUFSIZ], yc[BUFSIZ], zc[BUFSIZ];
		sscanf( cline.c_str(), "%s%s%s%s", &dummy, &xc, &yc, &zc );
		unsigned int id0 = atoi( xc ) - 1;
		unsigned int id1 = atoi( yc ) - 1;
		unsigned int id2 = atoi( zc ) - 1;
		mesh().setIndex( f_id, id0 ); ++f_id;
		mesh().setIndex( f_id, id1 ); ++f_id;
		mesh().setIndex( f_id, id2 ); ++f_id;
	      }
	  
	  }
      
      } // while
  
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
  
    ofs << "#$SMF 1.0" << std::endl;

    int vn = (int) ((float) mesh().points().size() / (float) nXYZ);
    ofs << "#$vertices " << vn << std::endl;

    int nn = (int) ((float) mesh().normals().size() / (float) nXYZ);
    if ( nn && isSaveNormal() )
      ofs << "#$normals " << nn << std::endl;

    int tn = (int) ((float) mesh().texcoords().size() / (float) mesh().n_tex());
    if ( tn && isSaveTexcoord() )
      ofs << "#$texcoords " << tn << std::endl;

#if 0
    int cn  = (int) ((float) mesh().colors().size() / (float) nXYZ);
    int cin = mesh().color_ids().size();
    if ( cn && isSaveColor() )
      {
	ofs << "#$colors " << cn << std::endl;
	if ( cin ) ofs << "#$color_ids " << cin << std::endl;
      }
#endif

    int fn = (int) ((float) mesh().indices().size() / (float) TRIANGLE);
    ofs << "#$faces " << fn << std::endl;
    ofs << "#" << std::endl;

    if ( vn )
      {
	std::vector<float>& points = mesh().points();
	for ( int i = 0; i < vn; ++i )
	  {
	    ofs << "v\t"
		<< points[ nXYZ * i ] << " "
		<< points[ nXYZ * i + 1 ]  << " "
		<< points[ nXYZ * i + 2 ]  << std::endl;
	  }
      }

    if ( nn && isSaveNormal() )
      {
	std::vector<float>& normals = mesh().normals();
	for ( int i = 0; i < nn; ++i )
	  {
	    ofs << "n\t"
		<< normals[ nXYZ * i ] << " "
		<< normals[ nXYZ * i + 1 ] << " "
		<< normals[ nXYZ * i + 2 ] << std::endl;
	  }
      }

    if ( tn && isSaveTexcoord() )
      {
	int n_tex = mesh().n_tex();
	std::vector<float>& texcoords = mesh().texcoords();
	for ( int i = 0; i < tn; ++i )
	  {
	    ofs << "r\t"
		<< texcoords[ n_tex * i ] << " "
		<< texcoords[ n_tex * i + 1 ];
	    if ( n_tex == 3 )
	      {
		ofs  << " " << texcoords[ n_tex * i + 2 ];
	      }
	    ofs << std::endl;
	  }
      }

#if 0
    if ( cn && isSaveColor() )
      {
	if ( mesh().colorAssigned() == ASSIGN_VERTEX )
	  {
	    ofs << "bind c vertex" << std::endl;
	  }
	else if ( mesh().colorAssigned() == ASSIGN_FACE )
	  {
	    ofs << "bind c face" << std::endl;
	  }

	// color
	std::vector<float>& colors = mesh().colors();
	for ( int i = 0; i < cn; ++i )
	  {
	    ofs << "c\t"
		<< colors[ nXYZ * i ] << " "
		<< colors[ nXYZ * i + 1 ] << " "
		<< colors[ nXYZ * i + 2 ] << std::endl;
	  }

	// color id
	std::vector<unsigned int>& color_ids = mesh().color_ids();
	for ( int i = 0; i < cin; ++i )
	  {
	    ofs << "cf\t" << color_ids[i] << endl;
	  }
      }
#endif

    if ( fn )
      {
	std::vector<unsigned int>& indices = mesh().indices();
	for ( int i = 0; i < fn; ++i )
	  {
	    ofs << "f\t" ;
	    for ( int j = 0; j < TRIANGLE; ++j )
	      {
		unsigned int id = indices[ TRIANGLE * i + j ] + 1;
		ofs << id;
		if ( nn && isSaveNormal() )   ofs << "//" << id;
		if ( tn && isSaveTexcoord() ) ofs << "//" << id;
		ofs << " " ;
	      }
	    ofs << std::endl;
	  }
      }

    ofs.close();

    return true;
  };

  bool outputToFile_Flat( const char* const filename ) {
    std::ofstream ofs( filename ); if ( !ofs ) return false;

    std::cout << "Output to file: " << filename << endl;

    mesh().printInfo();

    // header

    mesh().createFaceNormals();
    mesh().createFaceMates();

    std::vector<float> points_save;
    std::vector<float> normals_save;

    std::vector<float>& points = mesh().points();
    std::vector<float>& fnormals = mesh().fnormals();
    std::vector<unsigned int>& indices = mesh().indices();
    //for ( int i = 0; i < indices.size(); i ++ )
    for ( unsigned int i = 0; i < mesh().numFaces(); i ++ )
      {
	for ( int j = 0; j < TRIANGLE; ++j )
	  {
	    unsigned int id = indices[ TRIANGLE * i + j ];
	    points_save.push_back( points[ nXYZ * id ] );
	    points_save.push_back( points[ nXYZ * id + 1 ] );
	    points_save.push_back( points[ nXYZ * id + 2 ] );
	    normals_save.push_back( fnormals[ nXYZ * i ] );
	    normals_save.push_back( fnormals[ nXYZ * i + 1 ] );
	    normals_save.push_back( fnormals[ nXYZ * i + 2 ] );
	  }
      }

    ofs << "#$SMF 1.0" << std::endl;

    unsigned int vn = indices.size();
    ofs << "#$vertices " << vn << std::endl;

    unsigned int nn = indices.size();
    if ( nn && isSaveNormal() )
      ofs << "#$normals " << nn << std::endl;

    int fn = (int) ((float) mesh().indices().size() / (float) TRIANGLE);
    ofs << "#$faces " << fn << std::endl;
    ofs << "#" << std::endl;

    if ( vn )
      {
	for ( unsigned int i = 0; i < vn; ++i )
	  {
	    ofs << "v\t"
		<< points_save[ nXYZ * i ] << " "
		<< points_save[ nXYZ * i + 1 ]  << " "
		<< points_save[ nXYZ * i + 2 ]  << std::endl;
	  }
      }

    if ( nn && isSaveNormal() )
      {
	//std::vector<float>& normals = mesh().normals();
	for ( unsigned int i = 0; i < nn; ++i )
	  {
	    ofs << "n\t"
		<< normals_save[ nXYZ * i ] << " "
		<< normals_save[ nXYZ * i + 1 ] << " "
		<< normals_save[ nXYZ * i + 2 ] << std::endl;
	  }
      }

    if ( fn )
      {
	int vcount = 1;
	for ( unsigned int i = 0; i < mesh().numFaces(); ++i )
	  {
	    ofs << "f\t" ;
	    for ( int j = 0; j < TRIANGLE; ++j )
	      {
		ofs << vcount;
		if ( nn && isSaveNormal() )   ofs << "//" << vcount;
		ofs << " " ;
		++vcount;
	      }
	    ofs << std::endl;
	  }
      }

    if ( fn )
      {
	// std::vector<int>& face_mates = mesh().face_mates();
	for ( unsigned int i = 0; i < mesh().numFaces(); ++i )
	  {
	    unsigned int m[TRIANGLE];
	    mesh().getFaceMates( i, m );

	    ofs << "fm\t" ;
	    for ( int j = 0; j < TRIANGLE; ++j )
	      {
		ofs << m[j]+1;
		ofs << " " ;
	      }
	    ofs << std::endl;
	  }
      }

    ofs.close();

    return true;
  };

  bool outputToFile( const char* const filename,
                     bool isSaveNormal,
                     bool isSaveTexcoord, bool isSaveBLoop ) {
    setSaveNormal( isSaveNormal );
    setSaveTexcoord( isSaveTexcoord );
    setSaveBLoop( isSaveBLoop );
    return outputToFile( filename );
  };

  bool outputToFile_Flat( const char* const filename,
			  bool isSaveNormal,
			  bool isSaveTexcoord, bool isSaveBLoop ) {
    setSaveNormal( isSaveNormal );
    setSaveTexcoord( isSaveTexcoord );
    setSaveBLoop( isSaveBLoop );
    return outputToFile_Flat( filename );
  };

};

#endif // _OBJRIO_HXX
