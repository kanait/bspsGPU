////////////////////////////////////////////////////////////////////
//
// $Id: BSPS3TexGPU.hxx,v 1.5 2005/01/04 04:18:07 kanai Exp $
//
// Copyright (c) 2004-2005 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _BSPSTEXGPU_HXX
#define _BSPSTEXGPU_HXX 1


#include <vector>
using namespace std;

#include <Point3.h>
#include <Point4.h>
#ifdef VM_INCLUDE_NAMESPACE
using namespace kh_vecmath;
#endif // VM_INCLUDE_NAMESPACE

#define RGBA 4

class BSPSTexGPU {

public:
  
  BSPSTexGPU(){};
//   BSPSTexGPU( std::vector<float>& uknot, std::vector<float>& vknot, 
// 	       int n_ucp, int n_vcp, 
// 	       std::vector<Point3f>& cp ) {
//     init( uknot, vknot, n_ucp, n_vcp, cp ); 
//   };
  ~BSPSTexGPU(){};

  int n_patch() const { return n_patch_; };

  int max_n_ukv() const { return max_n_ukv_; };
  int n_ukv() const { return n_ukv_; };
  std::vector<float>& ukv() { return ukv_; };
  int max_w_ukt() const { return max_w_ukt_; };
  int max_h_ukt() const { return max_h_ukt_; };
  int w_ukt() const { return w_ukt_; };
  int h_ukt() const { return h_ukt_; };
  std::vector<float>& uknottex() { return uknottex_; };

  int max_n_vkv() const { return max_n_vkv_; };
  int n_vkv() const { return n_vkv_; };
  std::vector<float>& vkv() { return vkv_; };
  int max_w_vkt() const { return max_w_vkt_; };
  int max_h_vkt() const { return max_h_vkt_; };
  int w_vkt() const { return w_vkt_; };
  int h_vkt() const { return h_vkt_; };
  std::vector<float>& vknottex() { return vknottex_; };
  
  int max_n_cp() const { return max_n_cp_; };
  int n_ucp() const { return n_ucp_; };
  int n_vcp() const { return n_vcp_; };
  std::vector<float>& cptex() { return cptex_; };

  ///////////////////////////////////////////////////////////////////////////////////
  //
  // for BSPS
  //
  ///////////////////////////////////////////////////////////////////////////////////
  
  void init( std::vector<BSPS>& bsps ) {
    cout << "Convert to texture... ";
//     pu_ = 3; // degree is fixed to 3
//     pv_ = 3; // degree is fixed to 3
    n_patch_ = bsps.size();
    storeKnotVector( bsps );
    storeKnotTex( bsps );
    storeCpTex( bsps );
    cout << "done." << endl;
  };
  
  void storeKnotVector( std::vector<BSPS>& bsps ) {
    
    //
    // U knot vector
    //
    // calculate the maximum number of u knot vectors
    for ( int i = 0; i < bsps.size(); ++i )
      {
	if ( !i )
	  {
	    max_n_ukv_ = bsps[i].n_ukv();
	    continue;
	  }
	if ( max_n_ukv_ < bsps[i].n_ukv() )
	  max_n_ukv_ = bsps[i].n_ukv();
      }

    ukv_.resize( (max_n_ukv_ * n_patch_) * RGBA );

    // store to 2D texture ... max_n_ukv_ * n_patch_ 
    for ( int i = 0; i < bsps.size(); ++i )
      {
	std::vector<float>& ref_ukv = bsps[i].ukv();
	for ( int j = 0; j < ref_ukv.size(); ++j )
	  {
	    int n = RGBA * (max_n_ukv_ * i + j);
	    ukv_[n]   = ref_ukv[j];      // a knot
	    ukv_[n+1] = bsps[i].n_ukv(); // the number of knot vector
	    ukv_[n+2] = .0f;
	    ukv_[n+3] = .0f;
	  }
      }

//     for ( int i = 0; i < ukv_.size(); i+=4 )
//       cout << "ukv " << i << " " 
// 	   << ukv_[i] << " " 
// 	   << ukv_[i+1] << " " 
// 	   << ukv_[i+2] << " " 
// 	   << ukv_[i+3] << " " 
// 	   << endl;
  
    //
    // V knot vector
    //
    // calculate the maximum number of v knot vectors
    for ( int i = 0; i < bsps.size(); ++i )
      {
	if ( !i )
	  {
	    max_n_vkv_ = bsps[i].n_vkv();
	    continue;
	  }
	if ( max_n_vkv_ < bsps[i].n_vkv() )
	  max_n_vkv_ = bsps[i].n_vkv();
      }

    vkv_.resize( (max_n_vkv_ * n_patch_) * RGBA );
    
    // store to 2D texture ... max_n_vkv_ * n_patch_ 
    for ( int i = 0; i < bsps.size(); ++i )
      {
	std::vector<float>& ref_vkv = bsps[i].vkv();
	for ( int j = 0; j < ref_vkv.size(); ++j )
	  {
	    int n = RGBA * (max_n_vkv_ * i + j);
	    vkv_[n]   = ref_vkv[j];      // a knot
	    vkv_[n+1] = bsps[i].n_vkv(); // the number of knot vector
	    vkv_[n+2] = .0f;
	    vkv_[n+3] = .0f;
	  }
      }

//     for ( int i = 0; i < vkv_.size(); i+=4 )
//       cout << "vkv " << i << " " 
// 	   << vkv_[i] << " " 
// 	   << vkv_[i+1] << " " 
// 	   << vkv_[i+2] << " " 
// 	   << vkv_[i+3] << " " 
// 	   << endl;
  
  };

  void storeKnotTex( std::vector<BSPS>& bsps ) {

    // calculate the maximum number of knots and knot differences of u

    // 
    // w_ukt_(w_vkt_) の値
    //    - 3 次 ... w_ukt_ = 4 (12 (個の要素) / 3 (1 つのピクセルに 3 つずつ格納))
    //    - 5 次 ... w_ukt_ = 7 (knot: 10 個 ... 3 ピクセル, kd: 15 個 ... 4 ピクセル)
    //    max_w_ukt_ はその最大値をとる
    //
    // 初期値は 4 (BSP_CUBIC であると想定)
    max_w_ukt_ = 4;
    for ( int i = 0; i < bsps.size(); ++i )
      {
	pu_ = bsps[i].u_degree();
	if ( pu_ == BSP_QUINTIC ) max_w_ukt_ = 7;

	if ( !i )
	  {
	    max_h_ukt_ = bsps[i].n_ukv() - 2 * pu_;
	    continue;
	  }
	if ( max_h_ukt_ < (bsps[i].n_ukv() - 2 * pu_) )
	  max_h_ukt_ = bsps[i].n_ukv() - 2 * pu_;
      }

    uknottex_.resize( (max_h_ukt_ * max_w_ukt_ * n_patch_) * RGBA );

    // store to 2D texture ... (max_h_ukt_ * max_w_ukt_ ) * n_patch_ 
    for ( int h = 0; h < bsps.size(); ++h )
      {
	pu_ = bsps[h].u_degree();
// 	cout << "pu_ " << pu_ << endl;

	int h_ukt = bsps[h].n_ukv() - 2 * pu_;
	std::vector<float>& uknot = bsps[h].ukv();
	for ( int i = 0; i < h_ukt; ++i )
	  {
	    int n = RGBA * max_w_ukt_ * (max_h_ukt_ * h + i);

	    if ( pu_ == BSP_CUBIC )
	      {
		// knots
		uknottex_[n]    = uknot[i+1]; // u_(i-2) ... i + p(=3) - 2
		uknottex_[n+1]  = uknot[i+2]; // u_(i-1) ... i + p(=3) - 1
		uknottex_[n+2]  = uknot[i+3]; // u_i     ... i + p(=3)
		uknottex_[n+3]  = max_h_ukt_; // the maximum number of knots and knot differences
		uknottex_[n+4]  = uknot[i+4]; // u_(i+1) ... i + p(=3) + 1
		uknottex_[n+5]  = uknot[i+5]; // u_(i+2) ... i + p(=3) + 2
		uknottex_[n+6]  = uknot[i+6]; // u_(i+3) ... i + p(=3) + 3
		uknottex_[n+7]  = .0f;
	    
		// knot differences
		uknottex_[n+8]  = uknot[i+4] - uknot[i+3]; // u_(i+1) - u_i
		uknottex_[n+9]  = uknot[i+4] - uknot[i+2]; // u_(i+1) - u_(i-1)
		uknottex_[n+10] = uknot[i+5] - uknot[i+3]; // u_(i+2) - u_i
		uknottex_[n+11] = .0f;
		uknottex_[n+12] = uknot[i+4] - uknot[i+1]; // u_(i+1) - u_(i-2)
		uknottex_[n+13] = uknot[i+5] - uknot[i+2]; // u_(i+2) - u_(i-1)
		uknottex_[n+14] = uknot[i+6] - uknot[i+3]; // u_(i+3) - u_i
		uknottex_[n+15] = .0f;
	      }
	    else if ( pu_ == BSP_QUINTIC )
	      {
		// knots
		uknottex_[n]    = uknot[i+1];  // u_(i-4) ... i + p(=5) - 4
		uknottex_[n+1]  = uknot[i+2];  // u_(i-3) ... i + p(=5) - 3
		uknottex_[n+2]  = uknot[i+3];  // u_(i-2) ... i + p(=5) - 2
		uknottex_[n+3]  = uknot[i+4];  // u_(i-1) ... i + p(=5) - 1

		uknottex_[n+4]  = uknot[i+5];  // u_(i)   ... i + p(=5)
		uknottex_[n+5]  = uknot[i+6];  // u_(i+1) ... i + p(=5) + 1
		uknottex_[n+6]  = uknot[i+7];  // u_(i+2) ... i + p(=5) + 2
		uknottex_[n+7]  = uknot[i+8];  // u_(i+3) ... i + p(=5) + 3

		uknottex_[n+8]  = uknot[i+9];  // u_(i+4) ... i + p(=5) + 4
		uknottex_[n+9]  = uknot[i+10]; // u_(i+5) ... i + p(=5) + 5
		uknottex_[n+10] = max_w_ukt_;  // the maximum number of degree (4 or 7)
		uknottex_[n+11] = max_h_ukt_;  // the maximum number of knots and knot differences

		// knot differences
		uknottex_[n+12] = uknot[i+6]  - uknot[i+5]; // u_(i+1) - u_(i)
		uknottex_[n+13] = uknot[i+6]  - uknot[i+4]; // u_(i+1) - u_(i-1)
		uknottex_[n+14] = uknot[i+7]  - uknot[i+5]; // u_(i+2) - u_(i)
		uknottex_[n+15] = uknot[i+6]  - uknot[i+3]; // u_(i+1) - u_(i-2)

		uknottex_[n+16] = uknot[i+7]  - uknot[i+4]; // u_(i+2) - u_(i-1)
		uknottex_[n+17] = uknot[i+8]  - uknot[i+5]; // u_(i+3) - u_(i)
		uknottex_[n+18] = uknot[i+6]  - uknot[i+2]; // u_(i+1) - u_(i-3)
		uknottex_[n+19] = uknot[i+7]  - uknot[i+3]; // u_(i+2) - u_(i-2)

		uknottex_[n+20] = uknot[i+8]  - uknot[i+4]; // u_(i+3) - u_(i-1)
		uknottex_[n+21] = uknot[i+9]  - uknot[i+5]; // u_(i+4) - u_(i)
		uknottex_[n+22] = uknot[i+6]  - uknot[i+1]; // u_(i+1) - u_(i-4)
		uknottex_[n+23] = uknot[i+7]  - uknot[i+2]; // u_(i+2) - u_(i-3)

		uknottex_[n+24] = uknot[i+8]  - uknot[i+3]; // u_(i+3) - u_(i-2)
		uknottex_[n+25] = uknot[i+9]  - uknot[i+4]; // u_(i+4) - u_(i-1)
		uknottex_[n+26] = uknot[i+10] - uknot[i+5]; // u_(i+5) - u_(i)
		uknottex_[n+27] = .0f;
	      }
	  }
      }
	
    // calculate the maximum number of knots and knot differences of v
    max_w_vkt_ = 4; // 12 (個の要素) / 3 (1 つのピクセルに 3 つずつ格納)
    for ( int i = 0; i < bsps.size(); ++i )
      {
	pv_ = bsps[i].v_degree();
	if ( pv_ == BSP_QUINTIC ) max_w_vkt_ = 7;

	if ( !i )
	  {
	    max_h_vkt_ = bsps[i].n_vkv() - 2 * pv_;
	    continue;
	  }
	if ( max_h_vkt_ < (bsps[i].n_vkv() - 2 * pv_) )
	  max_h_vkt_ = bsps[i].n_vkv() - 2 * pv_;
      }

    vknottex_.resize( (max_h_vkt_ * max_w_vkt_ * n_patch_) * RGBA );

    // store to 2D texture ... (max_h_vkt_ * max_w_vkt_ ) * n_patch_ 
    for ( int h = 0; h < bsps.size(); ++h )
      {
	pv_ = bsps[h].v_degree();
	
	std::vector<float>& vknot = bsps[h].vkv();
	int h_vkt = bsps[h].n_vkv() - 2 * pv_;
	for ( int i = 0; i < h_vkt; ++i )
	  {
	    // knots
	    int n = RGBA * max_w_vkt_ * (max_h_vkt_ * h + i);

	    if ( pv_ == BSP_CUBIC )
	      {
		vknottex_[n]    = vknot[i+1]; // v_(i-2) ... i + p(=3) - 2
		vknottex_[n+1]  = vknot[i+2]; // v_(i-1) ... i + p(=3) - 1
		vknottex_[n+2]  = vknot[i+3]; // v_i     ... i + p(=3)
		vknottex_[n+3]  = max_h_vkt_; // the maximum number of knots and knot differences
		vknottex_[n+4]  = vknot[i+4]; // v_(i+1) ... i + p(=3) + 1
		vknottex_[n+5]  = vknot[i+5]; // v_(i+2) ... i + p(=3) + 2
		vknottex_[n+6]  = vknot[i+6]; // v_(i+3) ... i + p(=3) + 3
		vknottex_[n+7]  = .0f;
	    
		// knot differences
		vknottex_[n+8]  = vknot[i+4] - vknot[i+3]; // v_(i+1) - v_i
		vknottex_[n+9]  = vknot[i+4] - vknot[i+2]; // v_(i+1) - v_(i-1)
		vknottex_[n+10] = vknot[i+5] - vknot[i+3]; // v_(i+2) - v_i
		vknottex_[n+11] = .0f;
		vknottex_[n+12] = vknot[i+4] - vknot[i+1]; // v_(i+1) - v_(i-2)
		vknottex_[n+13] = vknot[i+5] - vknot[i+2]; // v_(i+2) - v_(i-1)
		vknottex_[n+14] = vknot[i+6] - vknot[i+3]; // v_(i+3) - v_i
		vknottex_[n+15] = .0f;
	      }
	    else if ( pv_ == BSP_QUINTIC )
	      {
		// knots
		vknottex_[n]    = vknot[i+1];  // u_(i-4) ... i + p(=5) - 4
		vknottex_[n+1]  = vknot[i+2];  // u_(i-3) ... i + p(=5) - 3
		vknottex_[n+2]  = vknot[i+3];  // u_(i-2) ... i + p(=5) - 2
		vknottex_[n+3]  = vknot[i+4];  // u_(i-1) ... i + p(=5) - 1

		vknottex_[n+4]  = vknot[i+5];  // u_(i)   ... i + p(=5)
		vknottex_[n+5]  = vknot[i+6];  // u_(i+1) ... i + p(=5) + 1
		vknottex_[n+6]  = vknot[i+7];  // u_(i+2) ... i + p(=5) + 2
		vknottex_[n+7]  = vknot[i+8];  // u_(i+3) ... i + p(=5) + 3

		vknottex_[n+8]  = vknot[i+9];  // u_(i+4) ... i + p(=5) + 4
		vknottex_[n+9]  = vknot[i+10]; // u_(i+5) ... i + p(=5) + 5
		vknottex_[n+10] = max_w_vkt_;  // the maximum number of degree (4 or 7)
		vknottex_[n+11] = max_h_vkt_;  // the maximum number of knots and knot differences

		// knot differences
		vknottex_[n+12] = vknot[i+6]  - vknot[i+5]; // u_(i+1) - u_(i)
		vknottex_[n+13] = vknot[i+6]  - vknot[i+4]; // u_(i+1) - u_(i-1)
		vknottex_[n+14] = vknot[i+7]  - vknot[i+5]; // u_(i+2) - u_(i)
		vknottex_[n+15] = vknot[i+6]  - vknot[i+3]; // u_(i+1) - u_(i-2)

		vknottex_[n+16] = vknot[i+7]  - vknot[i+4]; // u_(i+2) - u_(i-1)
		vknottex_[n+17] = vknot[i+8]  - vknot[i+5]; // u_(i+3) - u_(i)
		vknottex_[n+18] = vknot[i+6]  - vknot[i+2]; // u_(i+1) - u_(i-3)
		vknottex_[n+19] = vknot[i+7]  - vknot[i+3]; // u_(i+2) - u_(i-2)

		vknottex_[n+20] = vknot[i+8]  - vknot[i+4]; // u_(i+3) - u_(i-1)
		vknottex_[n+21] = vknot[i+9]  - vknot[i+5]; // u_(i+4) - u_(i)
		vknottex_[n+22] = vknot[i+6]  - vknot[i+1]; // u_(i+1) - u_(i-4)
		vknottex_[n+23] = vknot[i+7]  - vknot[i+2]; // u_(i+2) - u_(i-3)

		vknottex_[n+24] = vknot[i+8]  - vknot[i+3]; // u_(i+3) - u_(i-2)
		vknottex_[n+25] = vknot[i+9]  - vknot[i+4]; // u_(i+4) - u_(i-1)
		vknottex_[n+26] = vknot[i+10] - vknot[i+5]; // u_(i+5) - u_(i)
		vknottex_[n+27] = .0f;
	      }
	  }
      }
    
  };

  void storeCpTex( std::vector<BSPS>& bsps ) {

//     cout << "aaa" << endl;
    // calculate the maximum number of control points
    for ( int i = 0; i < bsps.size(); ++i )
      {
	if ( !i )
	  {
	    max_n_cp_ = bsps[i].n_cp();
	    continue;
	  }
	if ( max_n_cp_ < bsps[i].n_cp() )
	  max_n_cp_ = bsps[i].n_cp();
      }

    // 各行の最初のピクセルに情報を記述
    //    0   1        2
    // R  udeg  cp[0].x cp[1].x ...
    // G  vdeg  cp[0].y cp[1].y ...
    // B  ucp   cp[0].z cp[1].z ...
    // A  vcp    w[0]    w[1]   ...
    max_n_cp_ += 1;
    
//     cout << "max number of cp " << max_n_cp_ << endl;

//     cout << "bbb" << endl;
    // store to 2D texture ( max_n_cp_ * n_patch_ )
    cptex_.resize( (max_n_cp_ * (n_patch_)) * RGBA );
    for ( int i = 0; i < bsps.size(); ++i )
      {
	int n = RGBA * (max_n_cp_ * i);
	cptex_[n]   = bsps[i].u_degree(); // degree in u direction
	cptex_[n+1] = bsps[i].v_degree(); // degree in v direction
	cptex_[n+2] = bsps[i].n_ucp(); // the number of control points in u direction
	cptex_[n+3] = bsps[i].n_vcp(); // the number of control points in v direction
	
	std::vector<Point4f>& cp = bsps[i].cp();
	for ( int j = 0; j < cp.size(); ++j )
	  {
	    int n = RGBA * (max_n_cp_ * i + j + 1);
	    //cout << "cp count " << n << endl;
	    cptex_[n]   = cp[j].x;
	    cptex_[n+1] = cp[j].y;
	    cptex_[n+2] = cp[j].z;
	    cptex_[n+3] = cp[j].w; // weight
	  }
      }

//     for ( int i = 0; i < cptex_.size(); i+=4 )
//       {
// 	cout << "cp\t" << i << " "
// 	     << cptex_[i] << "\t" 
// 	     << cptex_[i+1] << "\t" 
// 	     << cptex_[i+2] << "\t" 
// 	     << cptex_[i+3] << " " << endl;
//       }
  };

#if 0
  void init( std::vector<float>& uknot, std::vector<float>& vknot, 
	     int n_ucp, int n_vcp, 
	     std::vector<Point3f>& cp ) {
    pu_ = 3; // degree is fixed to 3
    pv_ = 3; // degree is fixed to 3
    storeKnotVector( uknot, vknot );
    storeKnotTex( uknot, vknot );
    storeCpTex( n_ucp, n_vcp, cp );
  };
  
  void storeKnotVector( std::vector<float>& uknot, std::vector<float>& vknot ) {

    mu_ = uknot.size()-1;
    n_ukv_ = uknot.size();
    ukv_.resize( uknot.size() * RGBA );
    for ( int i = 0; i < uknot.size(); ++i ) 
      {
	ukv_[RGBA*i]   = uknot[i];
	ukv_[RGBA*i+1] = uknot[i];
	ukv_[RGBA*i+2] = uknot[i];
	ukv_[RGBA*i+3] = uknot[i];
      }

    mv_ = vknot.size()-1;
    n_vkv_ = vknot.size();
    vkv_.resize( vknot.size() * RGBA );
    for ( int i = 0; i < vknot.size(); ++i ) 
      {
	vkv_[RGBA*i]   = vknot[i];
	vkv_[RGBA*i+1] = vknot[i];
	vkv_[RGBA*i+2] = vknot[i];
	vkv_[RGBA*i+3] = vknot[i];
      }
  };

  //
  // 12 * (m + 1 - 2*(p - 1)) (p=3) sized texture
  //
  // the range of "span" ... i = p ... m - p (n+1)
  //
  void storeKnotTex( std::vector<float>& uknot, std::vector<float>& vknot ) {
    
    int n;
    h_ukt_ = uknot.size() - 2 * pu_; // span の範囲
    w_ukt_ = 4; // 12 (個の要素) / 3 (1 つのピクセルに 3 つずつ格納)
    uknottex_.resize( h_ukt_ * w_ukt_ * RGBA );
    n = RGBA*w_ukt_;
    for ( int i = 0; i < h_ukt_; ++i )
      {
	// knots
	uknottex_[n*i]    = uknot[i+1]; // u_(i-2) ... i + p(=3) - 2
	uknottex_[n*i+1]  = uknot[i+2]; // u_(i-1) ... i + p(=3) - 1
	uknottex_[n*i+2]  = uknot[i+3]; // u_i     ... i + p(=3)
	uknottex_[n*i+3]  = .0f;
	uknottex_[n*i+4]  = uknot[i+4]; // u_(i+1) ... i + p(=3) + 1
	uknottex_[n*i+5]  = uknot[i+5]; // u_(i+2) ... i + p(=3) + 2
	uknottex_[n*i+6]  = uknot[i+6]; // u_(i+3) ... i + p(=3) + 3
	uknottex_[n*i+7]  = .0f;
	
	// knot differences
	uknottex_[n*i+8]  = uknot[i+4] - uknot[i+3]; // u_(i+1) - u_i
	uknottex_[n*i+9]  = uknot[i+4] - uknot[i+2]; // u_(i+1) - u_(i-1)
	uknottex_[n*i+10] = uknot[i+5] - uknot[i+3]; // u_(i+2) - u_i
	uknottex_[n*i+11] = .0f;
	uknottex_[n*i+12] = uknot[i+4] - uknot[i+1]; // u_(i+1) - u_(i-2)
	uknottex_[n*i+13] = uknot[i+5] - uknot[i+2]; // u_(i+2) - u_(i-1)
	uknottex_[n*i+14] = uknot[i+6] - uknot[i+3]; // u_(i+3) - u_i
	uknottex_[n*i+15] = .0f;
      }

    h_vkt_ = vknot.size() - 2 * pv_; // span の範囲
    w_vkt_ = 4; // 12 (個の要素) / 3 (1 つのピクセルに 3 つずつ格納)
    vknottex_.resize( h_vkt_ * w_vkt_ * RGBA );
    n = RGBA*w_vkt_;
    for ( int i = 0; i < h_vkt_; ++i )
      {
	// knots
	vknottex_[n*i]    = vknot[i+1]; // u_(i-2) ... i + p(=3) - 2
	vknottex_[n*i+1]  = vknot[i+2]; // u_(i-1) ... i + p(=3) - 1
	vknottex_[n*i+2]  = vknot[i+3]; // u_i     ... i + p(=3)
	vknottex_[n*i+3]  = .0f;
	vknottex_[n*i+4]  = vknot[i+4]; // u_(i+1) ... i + p(=3) + 1
	vknottex_[n*i+5]  = vknot[i+5]; // u_(i+2) ... i + p(=3) + 2
	vknottex_[n*i+6]  = vknot[i+6]; // u_(i+3) ... i + p(=3) + 3
	vknottex_[n*i+7]  = .0f;
	
	// knot differences
	vknottex_[n*i+8]  = vknot[i+4] - vknot[i+3]; // u_(i+1) - u_i
	vknottex_[n*i+9]  = vknot[i+4] - vknot[i+2]; // u_(i+1) - u_(i-1)
	vknottex_[n*i+10] = vknot[i+5] - vknot[i+3]; // u_(i+2) - u_i
	vknottex_[n*i+11] = .0f;
	vknottex_[n*i+12] = vknot[i+4] - vknot[i+1]; // u_(i+1) - u_(i-2)
	vknottex_[n*i+13] = vknot[i+5] - vknot[i+2]; // u_(i+2) - u_(i-1)
	vknottex_[n*i+14] = vknot[i+6] - vknot[i+3]; // u_(i+3) - u_i
	vknottex_[n*i+15] = .0f;
      }

  };

  void storeCpTex( int n_ucp, int n_vcp, std::vector<Point3f>& cp ) {
    n_ucp_ = n_ucp;
    n_vcp_ = n_vcp;
    cptex_.resize( cp.size() * RGBA );
    for ( int i = 0; i < cp.size(); ++i )
      {
	cptex_[RGBA*i]   = cp[i].x;
	cptex_[RGBA*i+1] = cp[i].y;
	cptex_[RGBA*i+2] = cp[i].z;
	cptex_[RGBA*i+3] = .0f;
      }
  };
#endif

private:

  int n_patch_; // the number of patches

  int mu_; // ノットベクトルの数 (= uknot.size()-1)
  int mv_; // ノットベクトルの数 (= vknot.size()-1)

  int pu_; // 次数 (=3)
  int pv_; // 次数 (=3)

  int max_n_ukv_;
  int n_ukv_;
  std::vector<float> ukv_; // u knot vector

  int max_n_vkv_;
  int n_vkv_;
  std::vector<float> vkv_; // v knot vector

  int max_w_ukt_; 
  int max_h_ukt_; 
  int h_ukt_; // height of u knot texture
  int w_ukt_; // width  of u knot texture
  std::vector<float> uknottex_; // u knots 2D texture

  int max_w_vkt_; 
  int max_h_vkt_; 
  int h_vkt_; // height of v knot texture
  int w_vkt_; // width  of v knot texture
  std::vector<float> vknottex_; // v knots 2D texture

  int max_n_cp_;
  int n_ucp_;
  int n_vcp_;
  std::vector<float> cptex_; // control points 2D texture

};

#endif // _BSPS3TEXGPU_HXX
