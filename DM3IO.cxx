////////////////////////////////////////////////////////////////////
//
// $Id: DM3IO.cxx 2021/06/05 12:18:12 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#include "envDep.h"

#include "DM3IO.hxx"
#include "BSPS.hxx"

namespace {

// Any brep surface that provides a NURBS form (proxy or native).
static bool tryGetNurbForm( const ON_Surface* srf, ON_NurbsSurface& nbs )
{
  if ( nullptr == srf )
    return false;
  return srf->GetNurbForm( nbs ) > 0;
}

static void elevateNurbsForBsps( ON_NurbsSurface& nbs )
{
  if ( nbs.m_is_rat )
    cout << "Rational surface." << endl;

  if ( nbs.m_order[0] < 4 )
    nbs.IncreaseDegree( 0, 3 );
  if ( nbs.m_order[1] < 4 )
    nbs.IncreaseDegree( 1, 3 );

  if ( ( nbs.m_order[0] == 6 ) && ( nbs.m_order[1] != 6 ) )
    nbs.IncreaseDegree( 1, 5 );
  if ( ( nbs.m_order[0] != 6 ) && ( nbs.m_order[1] == 6 ) )
    nbs.IncreaseDegree( 0, 5 );
}

static void appendBspsPatchFromNurbs( ON_NurbsSurface& nbs, int patch_id, std::vector<BSPS>& bsps,
				      int& n_ucp, int& n_vcp )
{
  cout << "ON_NurbsSurface dim = " << nbs.m_dim << " is_rat = "
       << nbs.m_is_rat << " order = " << nbs.m_order[0] << " X " << nbs.m_order[1]
       << " cv_count = " << nbs.m_cv_count[0] << " X " << nbs.m_cv_count[1] << endl;
  cout << "Knots count =" << nbs.KnotCount( 0 ) << " X " << nbs.KnotCount( 1 ) << endl;

  BSPS bsp;
  bsp.setID( patch_id );

  bsp.setUdegree( nbs.m_order[0] - 1 );
  bsp.setVdegree( nbs.m_order[1] - 1 );

  double* knot = nbs.m_knot[0];
  bsp.addUkv( (float)knot[0] );
  for ( int i = 0; i < nbs.KnotCount( 0 ); ++i )
    bsp.addUkv( (float)knot[i] );
  bsp.addUkv( (float)knot[nbs.KnotCount( 0 ) - 1] );

  knot = nbs.m_knot[1];
  bsp.addVkv( (float)knot[0] );
  for ( int i = 0; i < nbs.KnotCount( 1 ); ++i )
    bsp.addVkv( (float)knot[i] );
  bsp.addVkv( (float)knot[nbs.KnotCount( 1 ) - 1] );

  n_ucp = nbs.m_cv_count[0];
  n_vcp = nbs.m_cv_count[1];

  bsp.setNUcp( nbs.m_cv_count[0] );
  bsp.setNVcp( nbs.m_cv_count[1] );

  for ( int i = 0; i < nbs.m_cv_count[0]; ++i )
    {
      for ( int j = 0; j < nbs.m_cv_count[1]; ++j )
	{
	  double* cv = nbs.CV( i, j );
	  double w = nbs.Weight( i, j );
	  bsp.addCp( Point4f( (float)cv[0], (float)cv[1], (float)cv[2], (float)w ) );
	}
    }

  bsps.push_back( bsp );
  cout << endl;
}

} // namespace

bool DM3IO::inputFromFile( const char* const filename, std::vector<BSPS>& bsps ) 
{
  ON::Begin();

  ON_TextLog dump_to_stdout;
  ON_TextLog* dump = &dump_to_stdout;

  const bool rc = model_->Read( filename, dump );

  if ( rc )
    dump->Print( "Successfully read.\n" );
  else
    dump->Print( "Errors during reading.\n" );
  
  int id = 0;
  int obj_index = 0;
  ONX_ModelComponentIterator cit( *model_, ON_ModelComponent::Type::ModelGeometry );
  for ( const ON_ModelComponent* mc = cit.FirstComponent(); nullptr != mc; mc = cit.NextComponent() )
    {
      const ON_ModelGeometryComponent* mgc = ON_ModelGeometryComponent::Cast( mc );
      if ( nullptr == mgc )
	continue;
      const ON_Geometry* geom = mgc->Geometry( nullptr );
      if ( nullptr == geom || geom->ObjectType() != ON::brep_object )
	continue;
      const ON_Brep* obj = ON_Brep::Cast( geom );
      if ( nullptr == obj )
	continue;

      for ( int si = 0; si < obj->m_S.Count(); si++ )
	{
	  const ON_Surface* srf = obj->m_S[si];
	  if ( nullptr == srf )
	    continue;

	  cout << "Name: " << srf->ClassId()->ClassName() << endl;

	  ON_NurbsSurface nbs;
	  if ( !tryGetNurbForm( srf, nbs ) )
	    {
	      cerr << "surface " << obj_index << " " << si
		   << " has no NURBS form (skipped)." << endl;
	      continue;
	    }

	  elevateNurbsForBsps( nbs );
	  appendBspsPatchFromNurbs( nbs, id, bsps, n_ucp_, n_vcp_ );
	  ++id;
	}
      ++obj_index;
    }

  cout << "the number of patches " << bsps.size() << endl;
  
  return rc;
}

void DM3IO::getSurfaceParameters( std::vector<Point3f>& cp,
				 std::vector<float>& ukv, std::vector<float>& vkv )
{
  int obj_index = 0;
  ONX_ModelComponentIterator cit( *model_, ON_ModelComponent::Type::ModelGeometry );
  for ( const ON_ModelComponent* mc = cit.FirstComponent(); nullptr != mc; mc = cit.NextComponent() )
    {
      const ON_ModelGeometryComponent* mgc = ON_ModelGeometryComponent::Cast( mc );
      if ( nullptr == mgc )
	continue;
      const ON_Geometry* geom = mgc->Geometry( nullptr );
      if ( nullptr == geom || geom->ObjectType() != ON::brep_object )
	continue;
      const ON_Brep* obj = ON_Brep::Cast( geom );
      if ( nullptr == obj )
	continue;

      for ( int si = 0; si < obj->m_S.Count(); si++ )
	{
	  const ON_Surface* srf = obj->m_S[si];
	  if ( nullptr == srf )
	    continue;

	  ON_NurbsSurface nbs;
	  if ( !tryGetNurbForm( srf, nbs ) )
	    {
	      cerr << "surface " << obj_index << " " << si
		   << " has no NURBS form (skipped)." << endl;
	      continue;
	    }

	  cout << "NURBS dim = " << nbs.m_dim << " is_rat = " << nbs.m_is_rat << " order = "
	       << nbs.m_order[0] << " X " << nbs.m_order[1] << " cv_count = " << nbs.m_cv_count[0]
	       << " X " << nbs.m_cv_count[1] << endl;
	  cout << "Knots count =" << nbs.KnotCount( 0 ) << " X " << nbs.KnotCount( 1 ) << endl;

	  double* knot = nbs.m_knot[0];
	  cout << "U dir" << endl;
	  ukv.push_back( (float)knot[0] );
	  for ( int i = 0; i < nbs.KnotCount( 0 ); ++i )
	    ukv.push_back( (float)knot[i] );
	  ukv.push_back( (float)knot[nbs.KnotCount( 0 ) - 1] );
	  for ( int i = 0; i < (int)ukv.size(); ++i )
	    cout << i << " " << ukv[i] << endl;

	  knot = nbs.m_knot[1];
	  cout << "V dir" << endl;
	  vkv.push_back( (float)knot[0] );
	  for ( int i = 0; i < nbs.KnotCount( 1 ); ++i )
	    vkv.push_back( (float)knot[i] );
	  vkv.push_back( (float)knot[nbs.KnotCount( 1 ) - 1] );
	  for ( int i = 0; i < (int)vkv.size(); ++i )
	    cout << i << " " << vkv[i] << endl;

	  n_ucp_ = nbs.m_cv_count[0];
	  n_vcp_ = nbs.m_cv_count[1];

	  for ( int i = 0; i < nbs.m_cv_count[0]; ++i )
	    {
	      for ( int j = 0; j < nbs.m_cv_count[1]; ++j )
		{
		  double* cv = nbs.CV( i, j );
		  Point3f p( (float)cv[0], (float)cv[1], (float)cv[2] );
		  cp.push_back( p );
		  cout << i << " " << j << " " << p << endl;
		}
	    }
	}
      ++obj_index;
    }
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

