////////////////////////////////////////////////////////////////////
//
// $Id: DM3IO.hxx,v 1.5 2005/01/05 14:07:53 kanai Exp $
//
// Copyright (c) 2004-2005 by Keio Research Institute at SFC
// All rights reserved. 
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
			     std::vector<float>& ukv, std::vector<float>& vkv )
  {
    int c = 0;
    
    for ( int i = 0; i < model_->m_object_table.Count(); i++ )
      {
	if ( model_->m_object_table[i].m_object->ObjectType() == ON::brep_object )
	  {
// 	    int si;
	    const ON_Brep* obj =  (ON_Brep*) model_->m_object_table[i].m_object;
	    for ( int si = 0; si < obj->m_S.Count(); si++ )
	      {
		const ON_Surface* srf = obj->m_S[si];
		if ( srf )
		  {
		    if ( strcmp( srf->ClassId()->ClassName(), "ON_NurbsSurface") )
		      {
			cerr << "surface " << i << " " << si << " is not ON_NurbsSurface. " << endl;
		      }
		    const char* s = srf->ClassId()->ClassName();

		    const ON_NurbsSurface* nbs = (ON_NurbsSurface*) srf;
		    cout << "ON_NurbsSurface dim = " << nbs->m_dim << " is_rat = " 
			 << nbs->m_is_rat << " order = " 
			 << nbs->m_order[0] << " X " << nbs->m_order[1] << " cv_count = " 
			 << nbs->m_cv_count[0] << " X " << nbs->m_cv_count[1]  << endl;

		    // degree

		    // Knot ‚Í knot vector ‚ÌŒ`‚ÅŠi”[‚³‚ê‚Ä‚¢‚é
		    cout << "Knots count =" << nbs->KnotCount(0) << " X " << nbs->KnotCount(1) 
			 << endl;

		    double* knot = nbs->m_knot[0];
		    cout << "U dir" << endl;

		    // m+1 ŒÂ‚ÌƒmƒbƒgƒxƒNƒgƒ‹‚ðŠi”[
		    ukv.push_back( knot[0] );
		    for ( int i  = 0; i < nbs->KnotCount(0); ++i )
		      {
			ukv.push_back( knot[i] );
			//cout << i << " " << knot[i] << endl;
		      }
		    ukv.push_back( knot[nbs->KnotCount(0)-1] );
		    for ( int i  = 0; i < ukv.size(); ++i )
		      cout << i << " " << ukv[i] << endl;

		    knot = nbs->m_knot[1];
		    cout << "V dir" << endl;
		     
		    // m+1 ŒÂ‚ÌƒmƒbƒgƒxƒNƒgƒ‹‚ðŠi”[
		    vkv.push_back( knot[0] );
		    for ( int i  = 0; i < nbs->KnotCount(1); ++i )
		      {
			vkv.push_back( knot[i] );
			//cout << i << " " << knot[i] << endl;
		      }
		    vkv.push_back( knot[nbs->KnotCount(1)-1] );

		    for ( int i  = 0; i < vkv.size(); ++i )
		      cout << i << " " << vkv[i] << endl;

		    //
		    // §Œä“_‚ÌŠi”[
		    //
		
		    n_ucp_ = nbs->m_cv_count[0];
		    n_vcp_ = nbs->m_cv_count[1];

		    for ( int i = 0; i < nbs->m_cv_count[0]; ++i )
		      {
			for ( int j = 0; j < nbs->m_cv_count[1]; ++j )
			  {
			    double* cv = nbs->CV( i, j );
			    Point3f p( cv[0], cv[1], cv[2] );
			    cp.push_back( p );
			    cout << i << " " << j << " " << p << endl;
			  }
		      }
		    
		  }

	      }
	  }
      }
  };

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

#endif // 

