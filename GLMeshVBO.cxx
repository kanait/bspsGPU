////////////////////////////////////////////////////////////////////
//
// $Id: GLMeshVBO.cxx 113 2005-09-24 21:25:09Z kanai $
//
//   OpenGL Mesh VBO draw class
//
// Copyright (c) 2005 by Takashi Kanai
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#include "envDep.h"

#include <GL/glew.h>

#include "MeshR.hxx"

#include "GLMeshVBO.hxx"

void GLMeshVBO::initVBO()
{
  usage_ = GL_STATIC_DRAW_ARB;
//   usage_ = GL_STREAM_DRAW_ARB;
//   usage_ = GL_DYNAMIC_DRAW_ARB;

  // vbo initialize
  std::vector<float>& points = mesh().points();
  ::glGenBuffersARB( 1, &vertex_vbo_ );
  ::glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertex_vbo_ );
  ::glBufferDataARB( GL_ARRAY_BUFFER_ARB, 
		     points.size() * sizeof(GL_FLOAT), 
		     &points[0], 
		     usage_ );
//   for ( int i = 0; i < points.size(); i+=3 )
//     {
//       cout << points[i] << " " << points[i+1] << " " << points[i+2] << endl;
//     }

  if ( mesh().normals().size() )
    {
      drawNormal_ = true;
      std::vector<float>& normals = mesh().normals();
      ::glGenBuffersARB( 1, &normal_vbo_ );
      ::glBindBufferARB( GL_ARRAY_BUFFER_ARB, normal_vbo_ );
      ::glBufferDataARB( GL_ARRAY_BUFFER_ARB, 
			 normals.size() * sizeof(GL_FLOAT), 
			 &normals[0], 
			 usage_ );
    }

  if ( mesh().texcoords().size() )
    {
      drawTexcoord_ = true;
      std::vector<float>& texcoords = mesh().texcoords();
      ::glGenBuffersARB( 1, &texcoord_vbo_ );
      ::glBindBufferARB( GL_ARRAY_BUFFER_ARB, texcoord_vbo_ );
      ::glBufferDataARB( GL_ARRAY_BUFFER_ARB, 
			 texcoords.size() * sizeof(GL_FLOAT), 
			 &texcoords[0], 
			 usage_ );
    }


  // ebo initialize
  std::vector<unsigned int>& indices = mesh().indices();
  ::glGenBuffersARB( 1, &ebo_ );
  ::glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, ebo_ );
  ::glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 
		     indices.size() * sizeof(GL_UNSIGNED_INT),
		     &indices[0], 
		     usage_ );
//   for ( int i = 0; i < indices.size(); ++i )
//     {
//       cout << "i = " << i << " " << indices[i] << endl;
//     }

  // get buffer size
  GLint v_size = 0;
  ::glGetBufferParameterivARB( GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &v_size );

  GLint e_size = 0;
  ::glGetBufferParameterivARB( GL_ELEMENT_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &e_size );
  std::cout << "Buffer size: vertex " << v_size << " element " << e_size << std::endl;
}

void GLMeshVBO::updateVBO()
{
//   ::glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertex_vbo_ );
  ::glDeleteBuffersARB( 1, &vertex_vbo_ );

  // vbo initialize
  std::vector<float>& points = mesh().points();
  ::glGenBuffersARB( 1, &vertex_vbo_ );
  ::glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertex_vbo_ );
  ::glBufferDataARB( GL_ARRAY_BUFFER_ARB, 
		     points.size() * sizeof(GL_FLOAT), 
		     &points[0], 
		     usage_ );
}

void GLMeshVBO::draw()
{
  ::glPushMatrix();
  if ( isDrawShading_ ) { drawShading(); }
  if ( isDrawWireframe_ ) { drawWireframe(); };
  ::glPopMatrix();
}

void GLMeshVBO::drawShading()
{
  if ( empty() ) return;
  
  if ( isSmoothShading_ == false ) ::glShadeModel( GL_FLAT );
  else                             ::glShadeModel( GL_SMOOTH );

//   ::glEnable( GL_NORMALIZE );
  ::glEnable( GL_CULL_FACE );
  
  ::glEnable( GL_LIGHTING );

  mtl_.bind();

//   ::glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );

  ::glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertex_vbo_ );
  ::glVertexPointer( 3, GL_FLOAT, 0, NULL );
  ::glEnableClientState( GL_VERTEX_ARRAY );

  if ( drawNormal_ ) 
    {
      ::glBindBufferARB( GL_ARRAY_BUFFER_ARB, normal_vbo_ );
      ::glNormalPointer( GL_FLOAT, 0, NULL );
      ::glEnableClientState( GL_NORMAL_ARRAY );
    }
  else 
    {
      ::glDisableClientState( GL_NORMAL_ARRAY );
    }
    
  if ( drawTexcoord_ ) 
    {
      ::glBindBufferARB( GL_ARRAY_BUFFER_ARB, texcoord_vbo_ );
      ::glTexCoordPointer( mesh().n_tex(), GL_FLOAT, 0, NULL );
      ::glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    }
  else
    {
      ::glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    }
    
  ::glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, ebo_ );
  ::glDrawElements( GL_TRIANGLES, mesh().indices().size(), GL_UNSIGNED_INT, NULL );

  ::glDisableClientState( GL_VERTEX_ARRAY );
  if ( drawNormal_ )   ::glDisableClientState( GL_NORMAL_ARRAY );
  if ( drawTexcoord_ ) ::glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  ::glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
  ::glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );

//   ::glPopClientAttrib();

  ::glDisable( GL_LIGHTING );

  ::glDisable( GL_CULL_FACE );
//   ::glDisable( GL_NORMALIZE );
}
  
void GLMeshVBO::drawWireframe()
{
  if ( empty() ) return;

  ::glColor3fv( wireColor() );
  ::glLineWidth( wireSize() );

  ::glEnableClientState( GL_VERTEX_ARRAY );
  ::glVertexPointer( 3, GL_FLOAT, 0, 
		     (GLfloat*) &(mesh_->points()[0]) );

  int numFaces = mesh_->numFaces();
  for ( int i = 0; i < numFaces; ++i )
    {
      ::glDrawElements( GL_LINE_LOOP, 3, GL_UNSIGNED_INT, 
			&(mesh_->indices()[3*i]) );
    }
}
  
