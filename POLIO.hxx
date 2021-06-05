////////////////////////////////////////////////////////////////////
//
// $Id: POLIO.hxx 2021/06/05 12:20:04 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _POLIO_HXX
#define _POLIO_HXX 1

#include "MeshRApp.hxx"

class POLIO : public MeshRApp {

public:

  POLIO() : MeshRApp() {};
  POLIO( MeshR& mesh ) : MeshRApp( mesh ) {};
  ~POLIO() {};

  bool inputFromFile( const char* const );
};

#endif // _POLIO_HXX

