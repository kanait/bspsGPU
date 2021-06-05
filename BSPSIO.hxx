////////////////////////////////////////////////////////////////////
//
// $Id: BSPSIO.hxx 2021/06/05 12:17:05 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
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

