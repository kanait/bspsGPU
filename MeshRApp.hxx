////////////////////////////////////////////////////////////////////
//
// $Id: MeshRApp.hxx 2021/06/05 12:27:15 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _MESHRAPP_HXX
#define _MESHRAPP_HXX 1

#include "MeshR.hxx"

class MeshRApp {

public:

  MeshRApp() : mesh_(NULL) {};
  MeshRApp( MeshR& mesh ) { setMesh( mesh ); };
  virtual ~MeshRApp(){};

  void clear() { if ( mesh_ ) deleteMesh(); };
  void setMesh( MeshR& mesh ) { mesh_ = &mesh; };
  MeshR& mesh() { return *mesh_; };
  
  void deleteMesh() { delete mesh_; mesh_ = NULL; };
  bool empty() const { return ( mesh_ != NULL ) ? false : true; };

private:

  MeshR* mesh_;

};

#endif // _MESHRAPP_HXX


