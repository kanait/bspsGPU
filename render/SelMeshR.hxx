////////////////////////////////////////////////////////////////////
//
// $Id: $
//
// Copyright (c) 2010 by Takashi Kanai. All rights reserved.
//
////////////////////////////////////////////////////////////////////

#ifndef _SELMESHR_HXX
#define _SELMESHR_HXX 1

#include <GL/gl.h>

#include <vector>
#include <set>
using namespace std;

#include "MeshR.hxx"
#include "GLMeshR.hxx"
#include "SelMesh.hxx"
#include "IDInfo.hxx"

class SelMeshR : public SelMesh {

public:

  SelMeshR() : isUseEye_(false) {};
  SelMeshR( GLPanel& glpane, MeshR& mesh, GLMeshR& glmesh,
            int mode ) : SelMesh( glpane ), isUseEye_(false) {
    setMesh( mesh );
    setGLMesh( glmesh );
    setMode( mode );
  };
  ~SelMeshR(){
    mesh().deleteSelected();
  };

  void init( GLPanel& glpane, MeshR& mesh, GLMeshR& glmesh, int mode ) {
    SelMesh::init( glpane );
    setMesh( mesh );
    setGLMesh( glmesh );
    setMode( mode );
  };

  void setMode( int mode ) {
    SelMesh::setMode( mode );
    if ( SelMesh::mode() == SEL_FACE )
      {
        mesh().setIsSelected( SEL_FACE );
        SelMesh::setSelectID( mesh().numFaces() );
      }
    else if ( SelMesh::mode() == SEL_VERTEX )
      {
        mesh().setIsSelected( SEL_VERTEX );
        SelMesh::setSelectID( mesh().numPoints() );
      }
    // isUseEye_ = true;
    isUseEye_ = false;
  };

  void setMesh( MeshR& mesh ) { mesh_ = &mesh; };
  MeshR& mesh() { return *mesh_; };

  void setGLMesh( GLMeshR& glmesh ) { glmesh_ = &glmesh; };
  GLMeshR& glmesh() { return *glmesh_; };

  void draw( int x, int y, std::vector<int>& selected_id ) {
    SelMesh::setCoord( x, y );
    SelMesh::setRectMode( false );
    draw( selected_id );
  };

  void draw( BBox2& bbox, std::vector<int>& selected_id ) {
    SelMesh::setBBox2( bbox );
    SelMesh::setRectMode( true );
    draw( selected_id );
  };

  void draw( std::vector<int>& selected_id ) {
    GLuint* selectBuf = draw_pick();

    GLint hits = ::glRenderMode( GL_RENDER );
    if ( hits == -1 ) // 何も選択されなかったとき
      {
        delete [] selectBuf;
        return;
      }

#if 0
    GLuint* ptr = selectBuf;
    for ( int i = 0; i < hits; ++i )
      {
        GLuint name = *ptr;
        // Skip
        ptr++; ptr++; ptr++;
        // 
        int id = *ptr;
        if ( (id >= 0) && ( id < maxID_ ) )
          {
            selected_id.push_back( *ptr );
            break;
          }
        ptr++;
      }
#endif

#if 1
    std::multiset< IDInfo > idinfo;
    GLuint* ptr = selectBuf;
    for ( int i = 0; i < hits; ++i )
      {
        GLuint name = *ptr;
        // 識別番号の階層の深さ

        ptr++;
        // デプスの最小値
        float depth = (float) *ptr/0x7fffffff;

        ptr++;
        // デプスの最大値

        ptr++;
        // 識別番号
        int id = *ptr;
        //        cout << "id " << id << endl;
        //        if ( (id >= 0) && ( id < maxID_ ) && (i == (hits-1)) )
        if ( (id >= 0) && ( id < maxID_ ) )
          {
            //selList.add(*ptr);
            idinfo.insert( IDInfo( id, depth ) );
          }
        ptr++;
      }

    // 最小デプス値のものだけを選択
    std::multiset<IDInfo>::iterator iiiter = idinfo.begin();
    selected_id.push_back( (*iiiter).id() );
#endif

    delete [] selectBuf;
  };

  void applySelected( std::vector<int>& selected, Point3f& eye ) {
    for ( unsigned int i = 0; i < selected.size(); ++i )
      {
        if ( SelMesh::mode() == SEL_FACE )
          {
            mesh().setFaceSelected( selected[i], true );
            SelMesh::countNumSelected();
          }
        else if ( SelMesh::mode() == SEL_VERTEX )
          {
            if ( mesh().vertexSelected( selected[i] ) == false )
              {
                mesh().setVertexSelected( selected[i], true );
                SelMesh::countNumSelected();
                std::cout << "Vertex No. " << selected[i] << " selected. count: "
                          << SelMesh::numSelected() << std::endl;
              }

#if 0
            if ( isUseEye_ )
              {
                Vector3f an;
                vt->calcNormal( an );
                Vector3f v1( eye - vt->point() );
                if ( an.dot( v1 ) > .0f )
                  {
                    vt->setSelected( true );
                    SelMesh::countNumSelected();
                  }
              }
            else
              {
                vt->setSelected( true );
                SelMesh::countNumSelected();
              }
#endif

          }
      }
  };

  void applySelected( std::vector<int>& selected ) {
    isUseEye_ = false;
    Point3f p;
    applySelected( selected, p );
  };

  void clearSelected() {
    mesh().clearSelected();
    SelMesh::clearNumSelected();
    cout << "all of selected elements were cleared." << endl;
  };

private:

  GLuint* draw_pick() {
    GLuint* selectBuf = SelMesh::initialize();

    ::glPushMatrix();

    SelMesh::setProjectionView();
    glpane().setModelView();

    if ( SelMesh::mode() == SEL_FACE )
      glmesh().drawFacesSelect();
    else if ( SelMesh::mode() == SEL_VERTEX )
      glmesh().drawPointsSelect();

    ::glPopMatrix();
    ::glFlush();

    return selectBuf;
  };

  MeshR* mesh_;
  GLMeshR* glmesh_;

  bool isUseEye_;

};

#endif // _SELMESHR_HXX
