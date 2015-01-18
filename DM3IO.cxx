////////////////////////////////////////////////////////////////////
//
// $Id: DM3IO.cxx,v 1.3 2005/01/05 14:07:53 kanai Exp $
//
// Copyright (c) 2004-2005 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#include "envDep.h"

#include "DM3IO.hxx"
#include "BSPS.hxx"

bool DM3IO::inputFromFile( const char* const filename, std::vector<BSPS>& bsps ) 
{
  ON::Begin();

  ON_TextLog dump_to_stdout;
  ON_TextLog* dump = &dump_to_stdout;

  FILE* archive_fp = ON::OpenFile( filename, "rb" );
  if ( !archive_fp ) 
    {
      dump->Print("  Unable to open file.\n" );
      return false;
    }

  // create achive object from file pointer
  ON_BinaryFile archive( ON::read3dm, archive_fp );

  // read the contents of the file into "model"
  bool rc = model_->Read( archive, dump );

  // close the file
  ON::CloseFile( archive_fp );

  // print diagnostic
  if ( rc )
    dump->Print("Successfully read.\n");
  else
    dump->Print("Errors during reading.\n");
  
  //
  // convert to BSPS
  //
  
//   int c = 0;
  int id = 0;
  for ( int i = 0; i < model_->m_object_table.Count(); i++ )
    {
      if ( model_->m_object_table[i].m_object->ObjectType() == ON::brep_object )
	{
// 	  int si;
	  const ON_Brep* obj =  (ON_Brep*) model_->m_object_table[i].m_object;
	  for ( int si = 0; si < obj->m_S.Count(); si++ )
	    {
	      const ON_Surface* srf = obj->m_S[si];
	      if ( srf )
		{
		  
		  cout << "Name: " << srf->ClassId()->ClassName() << endl;
		  
		  //const ON_NurbsSurface* nbs; 
		  ON_NurbsSurface nbs;
		  if ( !strcmp( srf->ClassId()->ClassName(), "ON_NurbsSurface") ||
		       !strcmp( srf->ClassId()->ClassName(), "ON_RevSurface") ||
		       !strcmp( srf->ClassId()->ClassName(), "ON_PlaneSurface") )
		    {
// 		      cout << srf->ClassId()->ClassName() << endl;		      
		      srf->GetNurbForm( nbs );
		      if ( nbs.m_is_rat )
			{
			  cout << "Rational surface." << endl;
// 			  nbs.MakeNonRational();
			}
		      if ( nbs.m_order[0] < 4 )
			{
			  nbs.IncreaseDegree( 0, 3 );
			}
		      if ( nbs.m_order[1] < 4 )
			{
			  nbs.IncreaseDegree( 1, 3 );
			}

		      // •Ðˆê•û‚ª 5 ŽŸ‚Ìê‡‚ÍC‚à‚¤ˆê•û‚à 5 ŽŸ‚Ü‚Åˆø‚«ã‚°‚é
		      if ( (nbs.m_order[0] == 6) && (nbs.m_order[1] != 6) )
			{
			  nbs.IncreaseDegree( 1, 5 );
			}

		      if ( (nbs.m_order[0] != 6) && (nbs.m_order[1] == 6) )
			{
			  nbs.IncreaseDegree( 0, 5 );
			}
		      //cout << "converted. " << endl;
		    }
		  else
		    {
		      cerr << "surface " << i << " " << si << " is not ON_NurbsSurface. " << endl;
		      continue;
		    }
		  

		  //		  const ON_NurbsSurface* nbs = (ON_NurbsSurface*) srf;
		  cout << "ON_NurbsSurface dim = " << nbs.m_dim << " is_rat = " 
		       << nbs.m_is_rat << " order = " 
		       << nbs.m_order[0] << " X " << nbs.m_order[1] << " cv_count = " 
		       << nbs.m_cv_count[0] << " X " << nbs.m_cv_count[1]  << endl;

		  // Knot ‚Í knot vector ‚ÌŒ`‚ÅŠi”[‚³‚ê‚Ä‚¢‚é
 		  cout << "Knots count =" << nbs.KnotCount(0) << " X " << nbs.KnotCount(1) 
 		       << endl;

		  BSPS bsp;
		  bsp.setID( id ); ++id;

		  bsp.setUdegree( nbs.m_order[0] - 1 );
		  bsp.setVdegree( nbs.m_order[1] - 1 );
// 		  cout << "degree " << bsp.u_degree() << " " << bsp.v_degree() << endl;

		  double* knot = nbs.m_knot[0];
// 		  cout << "U dir" << endl;

		  // m+1 ŒÂ‚ÌƒmƒbƒgƒxƒNƒgƒ‹‚ðŠi”[
		  //ukv.push_back( knot[0] );
		  bsp.addUkv( knot[0] );
		  for ( int i  = 0; i < nbs.KnotCount(0); ++i )
		    {
		      bsp.addUkv( knot[i] );
// 		      ukv.push_back( knot[i] );
		      //cout << i << " " << knot[i] << endl;
		    }
		  bsp.addUkv( knot[nbs.KnotCount(0)-1] );
		  //ukv.push_back( knot[nbs.KnotCount(0)-1] );
// 		  for ( int i  = 0; i < ukv.size(); ++i )
// 		    cout << i << " " << ukv[i] << endl;

		  knot = nbs.m_knot[1];
// 		  cout << "V dir" << endl;
		     
		  // m+1 ŒÂ‚ÌƒmƒbƒgƒxƒNƒgƒ‹‚ðŠi”[
		  bsp.addVkv( knot[0] );
// 		  vkv.push_back( knot[0] );
		  for ( int i  = 0; i < nbs.KnotCount(1); ++i )
		    {
		      bsp.addVkv( knot[i] );
// 		      vkv.push_back( knot[i] );
		      //cout << i << " " << knot[i] << endl;
		    }
		  bsp.addVkv( knot[nbs.KnotCount(1)-1] );
// 		  vkv.push_back( knot[nbs.KnotCount(1)-1] );

// 		  for ( int i  = 0; i < vkv.size(); ++i )
// 		    cout << i << " " << vkv[i] << endl;

		  //
		  // §Œä“_‚ÌŠi”[
		  //
		
		  n_ucp_ = nbs.m_cv_count[0];
		  n_vcp_ = nbs.m_cv_count[1];

		  bsp.setNUcp( nbs.m_cv_count[0] );
		  bsp.setNVcp( nbs.m_cv_count[1] );

		  for ( int i = 0; i < nbs.m_cv_count[0]; ++i )
		    {
		      for ( int j = 0; j < nbs.m_cv_count[1]; ++j )
			{
			  double* cv = nbs.CV( i, j );
			  double  w  = nbs.Weight( i, j );
			  Point4f p( cv[0], cv[1], cv[2], w );
			  bsp.addCp( p );
// 			  cp.push_back( p );
// 			  cout << i << " " << j << " " << p << endl;
			}
		    }

// 		  bsp.Print();
		  bsps.push_back( bsp );

		  cout << endl;

		}

	    }
	}
    }

  cout << "the number of patches " << bsps.size() << endl;
  
  return true;
}

void DM3IO::normalizeCp( std::vector<BSPS>& bsps, Point3f& center, float* maxlen ) {
    
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
	  //cout << i << " " << p1 << endl;
	}
    }
}


