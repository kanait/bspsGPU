////////////////////////////////////////////////////////////////////
//
// $Id: BSPS.hxx,v 1.6 2005/01/05 14:07:53 kanai Exp $
//
// Copyright (c) 2004-2005 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _BSPS_HXX
#define _BSPS_HXX 1

#include <vector>
using namespace std;

#include <Point4.h>
#ifdef VM_INCLUDE_NAMESPACE
using namespace kh_vecmath;
#endif // VM_INCLUDE_NAMESPACE

#include "MeshR.hxx"

// #include "MeshL.hxx"
// #include "VertexL.hxx"
// #include "FaceL.hxx"
// #include "HalfedgeL.hxx"
// #include "TexcoordL.hxx"

//
// the same one is defined on bsps.cg
//

#include "mydefine.h"

class BSPS {

public:

  BSPS() : u_degree_(3), v_degree_(3) {};
  ~BSPS(){};

  int id() const { return id_; };
  void setID( int i ) { id_ = i; };

  int u_degree() const { return u_degree_; };
  int v_degree() const { return v_degree_; };
  void setUdegree( int d ) { u_degree_ = d; };
  void setVdegree( int d ) { v_degree_ = d; };
  
  int n_ukv() const { return ukv_.size(); };
  int n_vkv() const { return vkv_.size(); };
  std::vector<float>& ukv() { return ukv_; };
  std::vector<float>& vkv() { return vkv_; };

  void addUkv( float t ) { ukv_.push_back( t ); };
  void addVkv( float t ) { vkv_.push_back( t ); };

  int n_ucp() const { return n_ucp_; };
  int n_vcp() const { return n_vcp_; };
  int n_cp() const { return cp_.size(); };
  void setNUcp( int n ) { n_ucp_ = n; };
  void setNVcp( int n ) { n_vcp_ = n; };

  void addCp( Point4f& p ) { cp_.push_back( p ); };
  std::vector<Point4f>& cp() { return cp_; };

  int findSpan( float t, int m, int deg, std::vector<float>& kv ) {
    
    if ( fabs( t - kv[m-deg] ) < 1.0e-05 ) return m-(deg+1);
    
    int low = deg;
    int high = m-deg;
    int mid = (int) ((low+high)/2);
    while ( t < kv[mid] || t >= kv[mid+1] )
      {
	if ( t < kv[mid] ) high = mid;
	else low = mid;
	mid = (int) ((low+high)/2);
      }
    return mid;
  };

  // for cubic degree B-spline

  int findSpan3( float t, int m, std::vector<float>& kv ) {
    
    if ( fabs( t - kv[m-3] ) < 1.0e-05 ) return m-4;
    
    int low = 3;
    int high = m-3;
    int mid = (int) ((low+high)/2);
    while ( t < kv[mid] || t >= kv[mid+1] )
      {
	if ( t < kv[mid] ) high = mid;
	else low = mid;
	mid = (int) ((low+high)/2);
      }
    return mid;
  };

  void fetchKnots3( int i, std::vector<float>& knot, std::vector<float>& kd,
		    std::vector<float>& kv ) {
    knot[0] = kv[i-2];
    knot[1] = kv[i-1];
    knot[2] = kv[i];
    knot[3] = kv[i+1];
    knot[4] = kv[i+2];
    knot[5] = kv[i+3];
//     cout << "knots " << knot[0] << " " << knot[1] << " " << knot[2] << " " << knot[3] << " " << knot[4] << " " << knot[5] << endl;
    kd[0]   = kv[i+1]-kv[i];
    kd[1]   = kv[i+1]-kv[i-1];
    kd[2]   = kv[i+2]-kv[i];
    kd[3]   = kv[i+1]-kv[i-2];
    kd[4]   = kv[i+2]-kv[i-1];
    kd[5]   = kv[i+3]-kv[i];
//     cout << "kd " << kd[0] << " " << kd[1] << " " << kd[2] << " " << kd[3] << " " << kd[4] << " " << kd[5] << endl;
  };

  void fetchCp3( int p, int q, std::vector<Point4f>& ocp, int n_vcp, std::vector<Point4f>& cp ) {
    for ( int i = 0; i < 4; ++i )
      {
	for ( int j = 0; j < 4; ++j )
	  {
	    int n = 4 * i + j;
	    ocp[n] = cp[ n_vcp * (p+i) + (q+j) ];
	  }
      }
  };

  void basisFunc3( float t, std::vector<float>& knot, std::vector<float>& kd,
		   std::vector<float>& Nu, std::vector<float>& dNu ) {
    float N[6];

    // p = 0
    N[0] = 1.0f;
  
    // p = 1
    N[1] = (knot[3] - t) * N[0] / kd[0];
    N[2] = (t - knot[2]) * N[0] / kd[0];
  
    // p = 2
    N[3] = (knot[3] - t) * N[1] / kd[1];
    N[4] = (t - knot[1]) * N[1] / kd[1] + (knot[4] - t) * N[2] / kd[2];
    N[5] = (t - knot[2]) * N[2] / kd[2];
  
    // p = 3
    Nu[0] = (knot[3] - t) * N[3] / kd[3];
    Nu[1] = (t - knot[0]) * N[3] / kd[3] + (knot[4] - t) * N[4] / kd[4];
    Nu[2] = (t - knot[1]) * N[4] / kd[4] + (knot[5] - t) * N[5] / kd[5];
    Nu[3] = (t - knot[2]) * N[5] / kd[5];
//     cout << "N " << Nu[0] << " " << Nu[1] << " " << Nu[2] << " " << Nu[3] << endl;
    
    // 1st order derivative
    dNu[0] =                      - 3.0f * N[3] / kd[3];
    dNu[1] =  3.0f * N[3] / kd[3] - 3.0f * N[4] / kd[4];
    dNu[2] =  3.0f * N[4] / kd[4] - 3.0f * N[5] / kd[5];
    dNu[3] =  3.0f * N[5] / kd[5];
  };

  void eval3( float u, float v, Point3f& pt, Vector3f& nrm ) {

//     cout << "knot vector size " << ukv_.size() << " " << vkv_.size() << endl;
    // U direction
    int uid = findSpan3( u, ukv_.size()-1, ukv_ );
//     cout << "uid " << uid << endl;

    std::vector<float> uknot(6);
    std::vector<float> ukd(6);
    fetchKnots3( uid, uknot, ukd, ukv_ );

    std::vector<float> Nu(4);
    std::vector<float> dNu(4);
    basisFunc3( u, uknot, ukd, Nu, dNu );
    
    // V direction
    int vid = findSpan3( v, vkv_.size()-1, vkv_ );
    std::vector<float> vknot(6);
    std::vector<float> vkd(6);
    fetchKnots3( vid, vknot, vkd, vkv_ );

    std::vector<float> Nv(4);
    std::vector<float> dNv(4);
    basisFunc3( v, vknot, vkd, Nv, dNv );
    
    // Control Points
    int iu = uid - 3;
    int iv = vid - 3;
//     cout << "u v " << u << " " << v << " " << "iu, iv " << iu << " " << iv << endl;
    std::vector<Point4f> cp(16);
    fetchCp3( iu, iv, cp, n_vcp_, cp_ );

    Point4f pt4( .0f, .0f, .0f, .0f );
    for ( int i = 0; i < 4; ++i )
      {
	for ( int j = 0; j < 4; ++j )
	  {
	    pt4 += (Nu[i] * Nv[j] * cp[4 * i + j]);
	  }
      }
    pt.set( pt4.x, pt4.y, pt4.z );

//     Vector3f pu( .0f, .0f, .0f );
    Point4f pu4( .0f, .0f, .0f, .0f );
    for ( int i = 0; i < 4; ++i )
      {
	for ( int j = 0; j < 4; ++j )
	  {
	    pu4 += (dNu[i] * Nv[j] * cp[4 * i + j]);
	  }
      }

    Point4f pv4( .0f, .0f, .0f, .0f );
    for ( int i = 0; i < 4; ++i )
      {
	for ( int j = 0; j < 4; ++j )
	  {
	    pv4 += (Nu[i] * dNv[j] * cp[4 * i + j]);
	  }
      }
    Vector3f pu( pu4.x, pu4.y, pu4.z );
    Vector3f pv( pv4.x, pv4.y, pv4.z );

    nrm.cross( pu, pv );
    nrm.normalize();

  };
  
  float paramU( int i, int udiv ) {
    return (ukv_[0] + ( ukv_[ukv_.size()-1] - ukv_[0] ) * (float) i / (float) udiv);
  };
    
  float paramV( int j, int vdiv ) {
    return (vkv_[0] + ( vkv_[vkv_.size()-1] - vkv_[0] ) * (float) j / (float) vdiv);
  };

#if 1
  void polygonize( MeshR& mesh, int udiv, int vdiv ) {
    
    // (udiv+1) * (vdiv+1) vertices

    unsigned int n_p = mesh.numPoints();

    for ( int i = 0; i <= udiv; ++i )
      {
	float u = paramU( i, udiv );
	for ( int j = 0; j <= vdiv; ++j )
	  {
	    float v = paramV( j, vdiv );

	    Point3f pt; Vector3f n;
	    eval3( u, v, pt, n );

	    mesh.addPoint( pt.x, pt.y, pt.z );

	    mesh.addNormal( n.x, n.y, n.z );

	    // b-spline face id is assigned to z component of texcoord.
	    float a = (float) id() / FID_DIV;
	    mesh.addTexcoord( u, v, a );
	  }
      }
    
    // udiv * vdiv * 2 triangles
    for ( int i = 0; i < udiv; ++i )
      {
	for ( int j = 0; j < vdiv; ++j )
	  {
	    // generate two triangle for each loop
	    unsigned int v0 = n_p + (vdiv+1) * (i)   + (j);
	    unsigned int v1 = n_p + (vdiv+1) * (i+1) + (j);
	    unsigned int v2 = n_p + (vdiv+1) * (i)   + (j+1);
	    mesh.addIndex( v0 );
	    mesh.addIndex( v1 );
	    mesh.addIndex( v2 );
	    
	    v0 = n_p + (vdiv+1) * (i+1) + (j+1);
	    v1 = n_p + (vdiv+1) * (i)   + (j+1);
	    v2 = n_p + (vdiv+1) * (i+1) + (j);
	    mesh.addIndex( v0 );
	    mesh.addIndex( v1 );
	    mesh.addIndex( v2 );
	    
	  }
      }
  };
#endif

#if 0
  void polygonize( MeshL& mesh, int udiv, int vdiv ) {
    
    // (udiv+1) * (vdiv+1) vertices
    std::vector<VertexL*> vtp;
    std::vector<NormalL*> nmp;
    std::vector<TexcoordL*> tcp;
    for ( int i = 0; i <= udiv; ++i )
      {
	float u = paramU( i, udiv );
	for ( int j = 0; j <= vdiv; ++j )
	  {
	    float v = paramV( j, vdiv );


	    Point3f pt; Vector3f n;
	    eval3( u, v, pt, n );

	    //cout << "u " << u << " v " << v << " " << pt << endl;
	    VertexL* vt = mesh.addVertex( pt ); 
	    vtp.push_back( vt );

	    NormalL* nm = mesh.addNormal( n ); 
	    nmp.push_back( nm );

	    // b-spline face id is assigned to z component of texcoord.
	    float a = (float) id() / FID_DIV;
	    TexcoordL* tc = mesh.addTexcoord( Point3f( u, v, a ) );
	    tcp.push_back( tc );
	  }
      }
    
    // udiv * vdiv * 2 triangles
    for ( int i = 0; i < udiv; ++i )
      {
	for ( int j = 0; j < vdiv; ++j )
	  {

	    FaceL* fc; HalfedgeL* he;
#if 0
	    // generate a rectangle for each loop
	    int v0, v1, v2, v3;
	    fc = mesh.addFace();
	    fc->setTexID( id() );
	    v0 = (vdiv+1) * (i)   + (j);
	    v1 = (vdiv+1) * (i)   + (j+1);
	    v2 = (vdiv+1) * (i+1) + (j+1);
	    v3 = (vdiv+1) * (i+1) + (j);
	    he = mesh.addHalfedge( fc, vtp[v0], nmp[v0], tcp[v0] );
	    he = mesh.addHalfedge( fc, vtp[v1], nmp[v1], tcp[v1] );
	    he = mesh.addHalfedge( fc, vtp[v2], nmp[v2], tcp[v2] );
	    he = mesh.addHalfedge( fc, vtp[v3], nmp[v3], tcp[v3] );
	    fc->calcNormal();
#endif

#if 1
	    // generate two triangle for each loop
	    int v0, v1, v2;
	    fc = mesh.addFace();
	    fc->setTexID( id() );
	    v0 = (vdiv+1) * (i)   + (j);
	    v1 = (vdiv+1) * (i+1) + (j);
	    v2 = (vdiv+1) * (i)   + (j+1);
	    he = mesh.addHalfedge( fc, vtp[v0], nmp[v0], tcp[v0] );
	    he = mesh.addHalfedge( fc, vtp[v1], nmp[v1], tcp[v1] );
	    he = mesh.addHalfedge( fc, vtp[v2], nmp[v2], tcp[v2] );
	    fc->calcNormal();
	    
	    fc = mesh.addFace();
	    fc->setTexID( id() );
	    v0 = (vdiv+1) * (i+1) + (j+1);
	    v1 = (vdiv+1) * (i)   + (j+1);
	    v2 = (vdiv+1) * (i+1) + (j);
	    he = mesh.addHalfedge( fc, vtp[v0], nmp[v0], tcp[v0] );
	    he = mesh.addHalfedge( fc, vtp[v1], nmp[v1], tcp[v1] );
	    he = mesh.addHalfedge( fc, vtp[v2], nmp[v2], tcp[v2] );
	    fc->calcNormal();
#endif

	  }
      }
  };
#endif

  void Print() {
    
    // u knots
    cout << "U knot vector: " << endl;
    for ( int i = 0; i < ukv_.size(); ++i )
      {
	cout << i << " " << ukv_[i] << endl;
      }
    cout << "V knot vector: " << endl;
    for ( int i = 0; i < vkv_.size(); ++i )
      {
	cout << i << " " << vkv_[i] << endl;
      }
    cout << "Control Points: " << endl;
    for ( int i = 0; i < cp_.size(); ++i )
      {
	cout << i << " " << cp_[i] << endl;
      }
  };
      
private:

  int id_;

  int u_degree_;
  int v_degree_;

  std::vector<float> ukv_;
  std::vector<float> vkv_;
  
  int n_ucp_;
  int n_vcp_;
  std::vector<Point4f> cp_;

};

#endif // _BSPS_HXX


