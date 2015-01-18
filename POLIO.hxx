////////////////////////////////////////////////////////////////////
//
// $Id: $
//
// Copyright (c) 2006 by Takashi Kanai. All rights reserved. 
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

