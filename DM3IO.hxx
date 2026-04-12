////////////////////////////////////////////////////////////////////
//
// $Id: DM3IO.hxx 2021/06/05 12:18:24 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _DM3IO_HXX
#define _DM3IO_HXX 1

#include "envDep.h"

#include <vector>
using namespace std;

#include <Point3.h>
#ifdef VM_INCLUDE_NAMESPACE
using namespace kh_vecmath;
#endif // VM_INCLUDE_NAMESPACE

#include "BSPS.hxx"

#include "opennurbs.h"
#include "opennurbs_extensions.h"
#include "opennurbs_model_geometry.h"

class DM3IO {

public:

  DM3IO( ONX_Model& model ) {
    setModel( model );
  };
  ~DM3IO(){};

  void setModel( ONX_Model& model ) { model_ = &model; };

  int n_ucp() const { return n_ucp_; };
  int n_vcp() const { return n_vcp_; };

  bool inputFromFile( const char* const, std::vector<BSPS>& );
  void normalizeCp( std::vector<BSPS>& bsps ) {
    Point3f p;
    float len;
    normalizeCp( bsps, p, &len );
  };
  void normalizeCp( std::vector<BSPS>&, Point3f&, float* );
  
  void getSurfaceParameters( std::vector<Point3f>& cp,
			     std::vector<float>& ukv, std::vector<float>& vkv );

  void normalizeCp( std::vector<Point3f>& cp )
  {
    Point3f vmax, vmin;

    for ( int i = 0; i < cp.size(); ++i )
      {
	if ( i )
	  {
	    if (cp[i].x > vmax.x) vmax.x = cp[i].x;
	    if (cp[i].x < vmin.x) vmin.x = cp[i].x;
	    if (cp[i].y > vmax.y) vmax.y = cp[i].y;
	    if (cp[i].y < vmin.y) vmin.y = cp[i].y;
	    if (cp[i].z > vmax.z) vmax.z = cp[i].z;
	    if (cp[i].z < vmin.z) vmin.z = cp[i].z;
	  }
	else
	  {
	    vmax.set( cp[i] ); vmin.set( cp[i] );
	  }
      }

    Point3f cen = vmax + vmin; cen.scale(.5);
    Point3f len = vmax - vmin;
    double maxlen = (std::fabs(len.x) > std::fabs(len.y) )
      ? std::fabs(len.x) : std::fabs(len.y); 
    maxlen = ( maxlen > std::fabs(len.z) ) ? maxlen : std::fabs(len.z);

    cout << "Normalized Cp" << endl;
    for ( int d = 0; d < cp.size(); ++d )
      {
	Point3f p1;
	p1.x = (cp[d].x - cen.x) / maxlen;
	p1.y = (cp[d].y - cen.y) / maxlen;
	p1.z = (cp[d].z - cen.z) / maxlen;
	cp[d].set( p1 );
	cout << d << " " << p1 << endl;
      }

  };

private:

  int n_ucp_;
  int n_vcp_;

  ONX_Model* model_;
  
};

#endif // _DM3IO_HXX

