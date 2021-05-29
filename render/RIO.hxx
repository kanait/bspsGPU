////////////////////////////////////////////////////////////////////
//
// $Id: $
//
// Copyright (c) 2005-2010 by Takashi Kanai. All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _RIO_HXX
#define _RIO_HXX 1

#include "MeshR.hxx"

class RIO {

public:

  RIO() { init(); };
  RIO( MeshR& mesh ) { init(); setMesh( mesh ); };
  virtual ~RIO() {};
  
  void init() {
    mesh_ = NULL;
    saveTexcoord_ = false;
    saveColor_ = false;
    saveNormal_ = false;
    saveBLoop_ = false;
  };
  
  MeshR& mesh() { return *mesh_; };
  bool empty() const { return ( mesh_ == NULL ) ? true : false; };
  void setMesh( MeshR& mesh ) { mesh_ = &mesh; };

  bool isSaveTexcoord() const { return saveTexcoord_; };
  void setSaveTexcoord( bool f ) { saveTexcoord_ = f; };

  bool isSaveColor() const { return saveColor_; };
  void setSaveColor( bool f ) { saveColor_ = f; };

  bool isSaveNormal() const { return saveNormal_; };
  void setSaveNormal( bool f ) { saveNormal_ = f; };

  bool isSaveBLoop() const { return saveBLoop_; };
  void setSaveBLoop( bool f ) { saveBLoop_ = f; };

private:

  MeshR* mesh_;

  bool saveTexcoord_;
  bool saveColor_;
  bool saveNormal_;
  bool saveBLoop_;

};

#endif // _RIO_HXX
