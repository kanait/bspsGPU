////////////////////////////////////////////////////////////////////
//
// Last Update: <2007-09-03 11:26:14 kanai>
//
// GLMeshVBO.hxx
// OpenGL Mesh VBO draw class
//
// Copyright (c) 2005-2007 by Takashi Kanai. All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _GLMESHVBO_HXX
#define _GLMESHVBO_HXX 1

#include "MeshR.hxx"
#include "GLMesh.hxx"

class GLMeshVBO : public GLMesh {

public:

  GLMeshVBO() : mesh_(NULL), drawNormal_(false), drawTexcoord_(false), drawColor_(false),
                vertex_vbo_(0), normal_vbo_(0), texcoord_vbo_(0), color_vbo_(0), shading_ebo_(0),
                wireframe_ebo_(0) {};
  ~GLMeshVBO() { clear(); };

  void setMesh( MeshR& mesh ) { mesh_ = &mesh; initVBO(); };
  void setMeshP( MeshR* mesh ) { mesh_ = mesh; initVBO(); };
  MeshR& mesh() { return *mesh_; };
  MeshR* meshP() { return mesh_; };
  bool empty() const { return ( mesh_ != NULL ) ? false : true; };
  void deleteMesh() { delete mesh_; mesh_ = NULL; };

  void initVBO() {
    usage_ = GL_STATIC_DRAW;
    //   usage_ = GL_STREAM_DRAW;
    //   usage_ = GL_DYNAMIC_DRAW;

    // vbo initialize
    std::vector<float>& points = mesh().points();
    ::glGenBuffers( 1, &vertex_vbo_ );
    ::glBindBuffer( GL_ARRAY_BUFFER, vertex_vbo_ );
    ::glBufferData( GL_ARRAY_BUFFER,
                       points.size() * sizeof(GLfloat),
                       &points[0],
                       usage_ );

    if ( mesh().normals().size() )
      {
        drawNormal_ = true;
        std::vector<float>& normals = mesh().normals();
        ::glGenBuffers( 1, &normal_vbo_ );
        ::glBindBuffer( GL_ARRAY_BUFFER, normal_vbo_ );
        ::glBufferData( GL_ARRAY_BUFFER, 
                           normals.size() * sizeof(GLfloat), 
                           &normals[0], 
                           usage_ );
      }

    if ( mesh().texcoords().size() )
      {
        drawTexcoord_ = true;
        std::vector<float>& texcoords = mesh().texcoords();
        cout << "texcoords " << texcoords.size() << endl;
        ::glGenBuffers( 1, &texcoord_vbo_ );
        ::glBindBuffer( GL_ARRAY_BUFFER, texcoord_vbo_ );
        ::glBufferData( GL_ARRAY_BUFFER, 
                           texcoords.size() * sizeof(GLfloat), 
                           &texcoords[0], 
                           usage_ );
      }

    if ( mesh().colors().size() )
      {
        drawColor_ = true;
        std::vector<unsigned char>& colors = mesh().colors();
        ::glGenBuffers( 1, &color_vbo_ );
        ::glBindBuffer( GL_ARRAY_BUFFER, color_vbo_ );
        ::glBufferData( GL_ARRAY_BUFFER, 
                           colors.size() * sizeof(GLubyte), 
                           &colors[0], 
                           usage_ );
      }


    // ebo for shading initialize
    std::vector<unsigned int>& indices = mesh().indices();
    ::glGenBuffers( 1, &shading_ebo_ );
    ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, shading_ebo_ );
    ::glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
                       indices.size() * sizeof(GLuint),
                       &indices[0], 
                       usage_ );

#if 0
    // indices for GL Lines
    int numFaces = mesh().numFaces();
    for ( int i = 0; i < numFaces; ++i )
      {
        windices_.push_back((mesh_->indices()[3*i]));
        windices_.push_back((mesh_->indices()[3*i+1]));
        windices_.push_back((mesh_->indices()[3*i+1]));
        windices_.push_back((mesh_->indices()[3*i+2]));
        windices_.push_back((mesh_->indices()[3*i+2]));
        windices_.push_back((mesh_->indices()[3*i]));
      }
#endif
    //mesh().createEdgesFromFaces();

    // ebo for wireframe initialize
    std::vector<unsigned int>& eindices = mesh().eindices();
    ::glGenBuffers( 1, &wireframe_ebo_ );
    ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, wireframe_ebo_ );
    ::glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                    eindices.size() * sizeof(GLuint),
                    &eindices[0],
                    usage_ );

    // get buffer size
    GLint v_size = 0;
    ::glGetBufferParameteriv( GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &v_size );

    GLint e_size = 0;
    ::glGetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &e_size );
    std::cout << "Buffer size: vertex " << v_size << " element " << e_size << std::endl;

    //windices_.clear();
  };

  void updateVBO() {
    //   ::glBindBuffer( GL_ARRAY_BUFFER, vertex_vbo_ );
    ::glDeleteBuffers( 1, &vertex_vbo_ );

    // vbo initialize
    std::vector<float>& points = mesh().points();
    ::glGenBuffers( 1, &vertex_vbo_ );
    ::glBindBuffer( GL_ARRAY_BUFFER, vertex_vbo_ );
    ::glBufferData( GL_ARRAY_BUFFER, 
                       points.size() * sizeof(GLfloat), 
                       &points[0], 
                       usage_ );
  };

  void clear() {
    if ( vertex_vbo_ ) ::glDeleteBuffers( 1, &vertex_vbo_ );
    if ( normal_vbo_ ) ::glDeleteBuffers( 1, &normal_vbo_ );
    if ( texcoord_vbo_ ) ::glDeleteBuffers( 1, &texcoord_vbo_ );
    if ( color_vbo_ ) ::glDeleteBuffers( 1, &color_vbo_ );
    if ( shading_ebo_ ) ::glDeleteBuffers( 1, &shading_ebo_ );
    if ( wireframe_ebo_ )
      {
        ::glDeleteBuffers( 1, &wireframe_ebo_ );
        // windices_.clear();
      }
    //if ( mesh_ ) deleteMesh(); 
  };

  void draw() {
    ::glPushMatrix();
    // if ( isDrawShading_ ) { drawShading(); }
    if ( isDrawWireframe_ ) { drawWireframe(); };
    ::glPopMatrix();
  };

  void drawShading() {

    if ( empty() ) return;

    if ( isSmoothShading_ == false ) ::glShadeModel( GL_FLAT );
    else
      ::glShadeModel( GL_SMOOTH );

    ::glEnable( GL_NORMALIZE );
    ::glEnable( GL_CULL_FACE );

    ::glEnable( GL_LIGHTING );

    mtl_.bind();

    //   ::glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );

    ::glBindBuffer( GL_ARRAY_BUFFER, vertex_vbo_ );
    ::glVertexPointer( 3, GL_FLOAT, 0, NULL );
    ::glEnableClientState( GL_VERTEX_ARRAY );

    if ( drawNormal_ ) 
      {
        ::glBindBuffer( GL_ARRAY_BUFFER, normal_vbo_ );
        ::glNormalPointer( GL_FLOAT, 0, NULL );
        ::glEnableClientState( GL_NORMAL_ARRAY );
      }
    else 
      {
        ::glDisableClientState( GL_NORMAL_ARRAY );
      }

    if ( drawTexcoord_ )
      {
        ::glBindBuffer( GL_ARRAY_BUFFER, texcoord_vbo_ );
        ::glTexCoordPointer( mesh().n_tex(), GL_FLOAT, 0, NULL );
        ::glEnableClientState( GL_TEXTURE_COORD_ARRAY );
      }
    else
      {
        ::glDisableClientState( GL_TEXTURE_COORD_ARRAY );
      }

    if ( drawColor_ )
      {
        ::glBindBuffer( GL_ARRAY_BUFFER, color_vbo_ );
        ::glColorPointer( 3, GL_UNSIGNED_BYTE, 0, NULL );
        ::glEnableClientState( GL_COLOR_ARRAY );
      }
    else
      {
        ::glDisableClientState( GL_COLOR_ARRAY );
      }

    ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, shading_ebo_ );
    ::glDrawElements( GL_TRIANGLES, mesh().indices().size(), GL_UNSIGNED_INT, NULL );

    ::glDisableClientState( GL_VERTEX_ARRAY );
    if ( drawColor_ )    ::glDisableClientState( GL_COLOR_ARRAY );
    if ( drawNormal_ )   ::glDisableClientState( GL_NORMAL_ARRAY );
    if ( drawTexcoord_ ) ::glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    ::glBindBuffer( GL_ARRAY_BUFFER, 0 );
    ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    //   ::glPopClientAttrib();

    ::glShadeModel( GL_FLAT );
    ::glDisable( GL_LIGHTING );

    ::glDisable( GL_CULL_FACE );
    ::glDisable( GL_NORMALIZE );
  };

  void drawWireframe() {
    if ( empty() ) return;

    ::glDisable( GL_LIGHTING );

    ::glColor3fv( wireColor() );
    ::glLineWidth( wireSize() );

    ::glBindBuffer( GL_ARRAY_BUFFER, vertex_vbo_ );
    ::glVertexPointer( 3, GL_FLOAT, 0, NULL );
    ::glEnableClientState( GL_VERTEX_ARRAY );

    ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, wireframe_ebo_ );
    ::glDrawElements( GL_LINES, mesh().eindices().size(), GL_UNSIGNED_INT, NULL );

    ::glDisableClientState( GL_VERTEX_ARRAY );
    ::glBindBuffer( GL_ARRAY_BUFFER, 0 );
    ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

#if 0
    ::glEnableClientState( GL_VERTEX_ARRAY );
    ::glVertexPointer( 3, GL_FLOAT, 0, 
                       (GLfloat*) &(mesh_->points()[0]) );

    int numFaces = mesh_->numFaces();
    for ( int i = 0; i < numFaces; ++i )
      {
        ::glDrawElements( GL_LINE_LOOP, 3, GL_UNSIGNED_INT, 
                          &(mesh_->indices()[3*i]) );
      }
#endif
  };


private:

  MeshR* mesh_;

  GLuint vertex_vbo_;
  GLuint normal_vbo_;
  GLuint texcoord_vbo_;
  GLuint color_vbo_;
  GLuint shading_ebo_;
  GLuint wireframe_ebo_;
  
  bool drawNormal_;
  bool drawTexcoord_;
  bool drawColor_;

  GLenum usage_;

  // indices for GL lines
  std::vector<unsigned int> windices_;

};

#endif // _GLMESHVBO_HXX
