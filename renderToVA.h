/*
  Render to vertex array class
  sgreen 12/2002
  
  - allocates array in video memory for VAR/PDR
  - does ReadPixels from float pbuffer to VAR memory using PDR
  - in theory this can happen entirely on the card, without any AGP transfers 

  to do:
  generalize
  use fences
*/

#ifndef _RENDERABLEVERTEXARRAY_H
#define _RENDERABLEVERTEXARRAY_H 1

#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

#include "pbuffer.h"

class RenderableVertexArray {

public:

  RenderableVertexArray(int nverts, int nattribs)
    : m_nverts(nverts), m_nattribs(nattribs)
  {
    m_format = 3; // RGB
    m_size = m_nverts * m_nattribs * m_format * sizeof(float); // 4 components, 4 bytes/float

    // allocate PDR/VAR memory
    m_data = (char *) wglAllocateMemoryNV(m_size, 0.0, 0.0, 1.0);  // this should give video mem
//    m_data = (char *) wglAllocateMemoryNV(m_size, 0.0, 0.0, 0.5);  // this should give agp mem
    if (!m_data) {
      fprintf(stderr, "Error allocating video memory\n");
      return;
    }

    // create vertex array range
    glVertexArrayRangeNV(m_size, m_data);

    // create pixel data range
    glPixelDataRangeNV(GL_READ_PIXEL_DATA_RANGE_NV, m_size, m_data);

    SetPointers();
    glEnableClientState(GL_READ_PIXEL_DATA_RANGE_NV);

    glPrimitiveRestartIndexNV(0xfff);

    m_id.resize( m_nverts );
    for ( int i = 0; i < m_nverts; ++i ) m_id[i] = i;
  }

  ~RenderableVertexArray()
  {
    wglFreeMemoryNV(m_data);
  }

  // set vertex array pointers
  void SetPointers()
  {
    glVertexPointer(m_format, GL_FLOAT, 0, m_data);
    glEnableClientState(GL_VERTEX_ARRAY);

    if ( m_nattribs > 1 )
      {
	glNormalPointer( GL_FLOAT, 0, m_data + (m_nverts * m_format * sizeof(float)) );
	glEnableClientState( GL_NORMAL_ARRAY );
      }

    glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);

    GLboolean valid;
    glGetBooleanv(GL_VERTEX_ARRAY_RANGE_VALID_NV, &valid);
    assert(valid);  
  }

  // read data from float pbuffer to position channel of vertex array 
//   void Read( CFPBuffer* pbuffer )
  void Read( PBuffer* pbuffer )
  {
    assert(m_nverts == pbuffer->GetWidth() * pbuffer->GetHeight());
//     pbuffer->activate();
    pbuffer->Activate();
    glReadPixels(0, 0, pbuffer->GetWidth(), pbuffer->GetHeight(), GL_RGB, GL_FLOAT, m_data);
//     pbuffer->deactivate();
    pbuffer->Deactivate();
  }

  // draw the geometry
  void Draw( GLenum mode, int count, int* indices )
  {
//    glFlushPixelDataRangeNV(GL_READ_PIXEL_DATA_RANGE_NV); // make sure read has finished

//    glEnableClientState(GL_PRIMITIVE_RESTART_NV);
    glDrawElements(mode, count, GL_UNSIGNED_INT, indices);
//    glDisableClientState(GL_PRIMITIVE_RESTART_NV);
  }

  // draw the geometry
  void Draw( GLenum mode )
  {
//    glFlushPixelDataRangeNV(GL_READ_PIXEL_DATA_RANGE_NV); // make sure read has finished

//    glEnableClientState(GL_PRIMITIVE_RESTART_NV);
    glDrawElements( mode, m_nverts, GL_UNSIGNED_INT, &m_id[0] );
//    glDisableClientState(GL_PRIMITIVE_RESTART_NV);
  }

private:

  std::vector<unsigned int> m_id;

  char *m_data;
  unsigned int m_size;
  unsigned int m_nverts;
  unsigned int m_nattribs;
  unsigned int m_format;
};

#endif // _RENDERABLEVERTEXARRAY_H
