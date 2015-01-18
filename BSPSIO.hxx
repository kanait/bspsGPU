////////////////////////////////////////////////////////////////////
//
// $Id: BSPSIO.hxx,v 1.3 2005/01/05 14:07:53 kanai Exp $
//
// Copyright (c) 2004-2005 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _BSPSIO_HXX
#define _BSPSIO_HXX 1

#include "envDep.h"

#include <vector>
using namespace std;

#include "BSPS.hxx"

class BSPSIO {

public:

  BSPSIO() {};
  ~BSPSIO() {};

  bool inputFromFile( const char* const, std::vector<BSPS>&  );
  void normalizeCp( std::vector<BSPS>& bsps ) {
    Point3f p;
    float len;
    normalizeCp( bsps, p, &len );
  };
  void normalizeCp( std::vector<BSPS>&, Point3f&, float* );

private:

};

#endif // _BSPSIO_HXX

