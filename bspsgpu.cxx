////////////////////////////////////////////////////////////////////
//
// $Id: bspsgpu.cxx 2021/06/05 12:09:33 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#include "envDep.h"

#include "GL/glew.h"
#if defined( _WIN32 )
#include "GL/wglew.h"
#endif

#include <GL/glut.h>

#include "GLShader.hxx"
#include "bspsgpu_embedded_shaders.h"

#include "mydef.h"
//#include "timer.hxx"
//#include "nvtimer.h"

#include <iostream>
#include <vector>
#include <list>
using namespace std;

#include <Point3.h>
#include <Vector3.h>
#ifdef VM_INCLUDE_NAMESPACE
using namespace kh_vecmath;
#endif // VM_INCLUDE_NAMESPACE

#include "GLPanel.hxx"
#include "PNGImage.hxx"
#include "GLMaterial.hxx"
#include "VWIO.hxx"
#include "SMFRIO.hxx"
#include "MeshR.hxx"
#include "GLMeshR.hxx"

#include "GLMeshVBO.hxx"
#include "DM3IO.hxx"
#include "BSPS.hxx"
#include "BSPSIO.hxx"
#include "BSPSTexGPU.hxx"
#include "GLBSPS.hxx"
#include "POLIO.hxx"

static float myLight[] = {
  0.0f, 0.0f, 100.0f, 1.0f,
  0.8f, 0.8f, 0.8f, 1.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  0.8f, 0.8f, 0.8f, 1.0f,
  1.0f, // 0.1f,
  0.0f,  // 0.05f
  0.0f  // 0.05f
};

static float myMatl[] = {
  0.2f, 0.2f, 0.2f, 1.0f, 
  0.8f, 0.8f, 0.8f, 1.0f, 
  0.0f, 0.0f, 0.0f, 1.0f, 
  0.8f, 0.8f, 0.8f, 1.0f, 
  80.0f
};

////////////////////////////////////////////////////////////////////////////////////

#define RENDER_TO_TEXTURE 1
#define USE_FRAG_PROGRAM 1
// #define USE_VAR 1
// #define USE_OLD 1
// #define USE_VBO 1 // currently it does not work

#ifdef RENDER_TO_TEXTURE
#include "RenderTextureFBO.h"
#endif

#ifdef USE_VBO
#include "renderVertexArray.h"
#endif

#ifdef USE_VAR
#include "renderToVA.h"
#endif

////////////////////////////////////////////////////////////////////////////////////

int width = 1024;
int height = 1024;
// int width = 512;
// int height = 512;

GLPanel pane;
GLMaterial mtl;

#include "mydefine.h"

unsigned short fGPUflag = DISPLAY_FRAGGPU;
//unsigned short fGPUflag = DISPLAY_POLYGON;
bool wfflag = false;
bool pwfflag = false;

#include "c11timer.hxx"
C11Timer c11fps;
double max_c11fps = 0.0;

char typebuf[BUFSIZ];
char txt[BUFSIZ];

////////////////////////////////////////////////////////////////////////////////////

int pbuffer_width  = width;
int pbuffer_height = height;
// int pbuffer_width  = 32;
// int pbuffer_height = 32;

int calc_width = pbuffer_width;
int calc_height = pbuffer_height;

#ifdef RENDER_TO_TEXTURE
RenderTextureFBO* pbuffer = NULL;
GLenum texTarget0 = GL_TEXTURE_RECTANGLE_EXT;
GLenum texInternalFormat0 = GL_RGBA32F_ARB;
GLenum texFormat0 = GL_RGBA;
GLenum texType0 = GL_FLOAT;
#endif

// GLSL programs (replaces NVIDIA Cg)
static GLuint s_progBsps = 0;
static GLuint s_progIsophoto = 0;

static GLuint s_texUKnotVec = 0;
static GLuint s_texUKnotTex = 0;
static GLuint s_texVKnotVec = 0;
static GLuint s_texVKnotTex = 0;
static GLuint s_texCp = 0;
static GLuint s_texLight = 0;

struct BspsUniformLocs
{
  GLint eyePos;
  GLint lightPos;
  GLint ambientColor;
  GLint diffuseColor;
  GLint specularColor;
  GLint specularShininess;
  GLint drawMode;
  GLint ukvTex;
  GLint uknotTex2D;
  GLint vkvTex;
  GLint vknotTex2D;
  GLint cpTex;
  GLint lightTex;
};

static BspsUniformLocs s_uBsps;

struct IsoUniformLocs
{
  GLint lightPos;
};

static IsoUniformLocs s_uIso;

#ifdef RENDER_TO_TEXTURE
GLuint position;
#endif

#ifdef USE_VBO
RenderVertexArray* rva = NULL;
#endif

#ifdef USE_VAR
RenderableVertexArray* var = NULL;
#endif

////////////////////////////////////////////////////////////////////////////////////

// $B%F%/%9%A%c3JG<$N$?$a$N$b$H$N%G!<%?(B
std::vector<Point3f> cp;
std::vector<float> ukv;
std::vector<float> vkv;

#ifdef USE_OLD
// $BI=<(MQ(B points
std::vector<Point3f> pt;
std::vector<Vector3f> nm;
#endif // USE_OLD

MeshR mesh;
std::vector<BSPS> bsps;
BSPSTexGPU bspstex;
GLMeshVBO glmeshvbo;
GLMeshR glmeshr;

////////////////////////////////////////////////////////////////////////////////////

void checkGLErrors(char *s)
{
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    fprintf(stderr, "%s: error - %s\n", s, (char *) gluErrorString(error));
  }
}

static void cacheBspsUniforms()
{
  GLuint p = s_progBsps;
  s_uBsps.eyePos = glGetUniformLocation( p, "eyePos" );
  s_uBsps.lightPos = glGetUniformLocation( p, "lightPos" );
  s_uBsps.ambientColor = glGetUniformLocation( p, "ambientColor" );
  s_uBsps.diffuseColor = glGetUniformLocation( p, "diffuseColor" );
  s_uBsps.specularColor = glGetUniformLocation( p, "specularColor" );
  s_uBsps.specularShininess = glGetUniformLocation( p, "specularShininess" );
  s_uBsps.drawMode = glGetUniformLocation( p, "drawMode" );
  s_uBsps.ukvTex = glGetUniformLocation( p, "ukvTex" );
  s_uBsps.uknotTex2D = glGetUniformLocation( p, "uknotTex2D" );
  s_uBsps.vkvTex = glGetUniformLocation( p, "vkvTex" );
  s_uBsps.vknotTex2D = glGetUniformLocation( p, "vknotTex2D" );
  s_uBsps.cpTex = glGetUniformLocation( p, "cpTex" );
  s_uBsps.lightTex = glGetUniformLocation( p, "lightTex" );
}

static void cacheIsoUniforms()
{
  GLuint p = s_progIsophoto;
  s_uIso.lightPos = glGetUniformLocation( p, "lightPos" );
}

void initShaders()
{
  using namespace bspsgpu_embedded;
  s_progBsps = createProgramFromSources( bsps_vert_glsl, "bsps.vert.glsl (embedded)",
                                         bsps_frag_glsl, "bsps.frag.glsl (embedded)" );
  s_progIsophoto = createProgramFromSources( isophoto_vert_glsl, "isophoto.vert.glsl (embedded)",
                                             isophoto_frag_glsl, "isophoto.frag.glsl (embedded)" );
  if ( !s_progBsps || !s_progIsophoto )
    {
      std::cerr << "Shader compile/link failed (embedded GLSL)." << std::endl;
      exit( 1 );
    }
  cacheBspsUniforms();
  cacheIsoUniforms();
}

void destroyShaders()
{
  if ( s_progBsps )
    glDeleteProgram( s_progBsps );
  if ( s_progIsophoto )
    glDeleteProgram( s_progIsophoto );
  s_progBsps = s_progIsophoto = 0;
}

////////////////////////////////////////////////////////////////////////////////////

/* Rectangle float RGBA: GL_FLOAT_RGBA_NV is NVIDIA-only and breaks macOS (incomplete / "unloadable" texture). */
static GLint bspsRectFloatRGBAInternalFormat()
{
  if ( GLEW_ARB_texture_float || GLEW_VERSION_3_0 )
    return GL_RGBA32F;
#ifdef GL_FLOAT_RGBA_NV
  if ( glewIsSupported( "GL_NV_float_buffer" ) )
    return GL_FLOAT_RGBA_NV;
#endif
  return GL_RGBA32F;
}

#ifndef GL_TEXTURE_RECTANGLE_ARB
#define GL_TEXTURE_RECTANGLE_ARB GL_TEXTURE_RECTANGLE_NV
#endif

static GLenum bspsRectTexTarget()
{
#if defined( GL_TEXTURE_RECTANGLE )
  return GL_TEXTURE_RECTANGLE;
#else
  return GL_TEXTURE_RECTANGLE_ARB;
#endif
}

void setTexture( BSPSTexGPU& bspstex )
{
  const GLenum rectTarget = bspsRectTexTarget();
  const GLint rectFloatRGBA = bspsRectFloatRGBAInternalFormat();

  // U Knot Vector
  glGenTextures( 1, &s_texUKnotVec );
  glBindTexture( rectTarget, s_texUKnotVec );
  glTexParameteri( rectTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( rectTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  std::vector<float>& ukv = bspstex.ukv();
  glTexImage2D( rectTarget, 0, rectFloatRGBA, bspstex.max_n_ukv(), bspstex.n_patch(), 0, GL_RGBA, GL_FLOAT,
                &ukv[0] );

  // U Knot Texture
  glGenTextures( 1, &s_texUKnotTex );
  glBindTexture( rectTarget, s_texUKnotTex );
  glTexParameteri( rectTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( rectTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  std::vector<float>& ukt = bspstex.uknottex();
  glTexImage2D( rectTarget, 0, rectFloatRGBA, bspstex.max_h_ukt() * bspstex.max_w_ukt(), bspstex.n_patch(), 0,
                GL_RGBA, GL_FLOAT, &ukt[0] );

  // V Knot Vector
  glGenTextures( 1, &s_texVKnotVec );
  glBindTexture( rectTarget, s_texVKnotVec );
  glTexParameteri( rectTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( rectTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  std::vector<float>& vkv = bspstex.vkv();
  glTexImage2D( rectTarget, 0, rectFloatRGBA, bspstex.max_n_vkv(), bspstex.n_patch(), 0, GL_RGBA, GL_FLOAT,
                &vkv[0] );

  // V Knot Texture
  glGenTextures( 1, &s_texVKnotTex );
  glBindTexture( rectTarget, s_texVKnotTex );
  glTexParameteri( rectTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( rectTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  std::vector<float>& vkt = bspstex.vknottex();
  glTexImage2D( rectTarget, 0, rectFloatRGBA, bspstex.max_h_vkt() * bspstex.max_w_vkt(), bspstex.n_patch(), 0,
                GL_RGBA, GL_FLOAT, &vkt[0] );

  // Control Points
  glGenTextures( 1, &s_texCp );
  glBindTexture( rectTarget, s_texCp );
  glTexParameteri( rectTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( rectTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  std::vector<float>& cp = bspstex.cptex();
  glTexImage2D( rectTarget, 0, rectFloatRGBA, bspstex.max_n_cp(), bspstex.n_patch(), 0, GL_RGBA, GL_FLOAT,
                &cp[0] );

  // Light Texture
  glGenTextures( 1, &s_texLight );
  glBindTexture( GL_TEXTURE_2D, s_texLight );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

  PNGImage pngimage;
  std::vector<unsigned char> img;
  pngimage.inputFromFile( "lattice.png", img );

  if ( !img.empty() )
    {
      GLint format;
      if ( pngimage.channel() == 3 )
        format = GL_RGB;
      else
        format = GL_RGBA;

      std::vector<float> fpng;

      for ( int i = 0; i < (int)img.size(); ++i )
        {
          float a = (float)img[i] / 255.0;
          fpng.push_back( a );
        }
      glTexImage2D( GL_TEXTURE_2D, 0, format, pngimage.w(), pngimage.h(), 0, format, GL_FLOAT,
                    &( fpng[0] ) );
    }
  else
    {
      cerr << "Error." << endl;
    }

  glBindTexture( rectTarget, 0 );
  glBindTexture( GL_TEXTURE_2D, 0 );
}

////////////////////////////////////////////////////////////////////////////////////

void initFPBuffer()
{
#if 0
  pbuffer = new RenderTextureFBO( calc_width, calc_height, 
				  texTarget0, texInternalFormat0, texFormat0, texType0, 
				  1 );

  pbuffer->Activate();

#ifdef USE_OLD
  glEnable( GL_DEPTH_TEST );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
#else
  pane.initGL();
#endif

  pbuffer->Deactivate();
#endif

}

////////////////////////////////////////////////////////////////////////////////////

void MakeGlutWindowCurrent()
{
  const int win = ::glutGetWindow();
  if ( win > 0 )
    ::glutSetWindow( win );
}

////////////////////////////////////////////////////////////////////////////////////

void displayPolygons()
{
  pane.clear( calc_width, calc_height );
  pane.setView();

  if ( fGPUflag == DISPLAY_ISOPHOTO )
    {
      float current_light[3];
      pane.getRealLightPosition( current_light );

      glUseProgram( s_progIsophoto );
      glUniform3fv( s_uIso.lightPos, 1, current_light );
    }
  else
    {
      pane.setLight();
    }
  
  glmeshvbo.draw();
//   glmeshr.draw();

  if ( fGPUflag == DISPLAY_ISOPHOTO )
    {
      glUseProgram( 0 );
    }
  else
    {
      if ( wfflag )
	{
	  for ( int i = 0; i < bsps.size(); ++i )
	    {
	      GLBSPS glbsps( bsps[i] );
	      glbsps.setWireSize( .3f );
	      glbsps.drawWireframe();
	    }
	}
    }

}

////////////////////////////////////////////////////////////////////////////////////

void displayFragmentGPU()
{
  ////////////////////////////////////////////////////////////////////////////////////
  //
  // 1st pass
  //
  ////////////////////////////////////////////////////////////////////////////////////

#ifdef USE_FRAG_PROGRAM

//   pbuffer->Activate();

#ifndef USE_OLD
  pane.setView();
  pane.setLight();
  
#else
  glViewport( 0, 0, calc_width, calc_height );
  glClearColor( .0f, .0f, .0f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
#endif

  float current_eye[3];
  pane.getRealViewPoint( current_eye );
//   cout << "eye " << current_eye[0] << " " << current_eye[1] << " " << current_eye[2] << endl;

  float current_light[3];
  pane.getRealLightPosition( current_light );
//   cout << "light " << current_light[0] << " " << current_light[1] << " " << current_light[2] << endl;
  glUseProgram( s_progBsps );

  glUniform3fv( s_uBsps.eyePos, 1, current_eye );
  glUniform3fv( s_uBsps.lightPos, 1, current_light );
  glUniform3fv( s_uBsps.ambientColor, 1, &( myMatl[0] ) );
  glUniform3fv( s_uBsps.diffuseColor, 1, &( myMatl[4] ) );
  glUniform3fv( s_uBsps.specularColor, 1, &( myMatl[12] ) );
  glUniform1f( s_uBsps.specularShininess, myMatl[16] );
  glUniform1f( s_uBsps.drawMode, (float)fGPUflag );

  const GLenum rectTarget = bspsRectTexTarget();

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( rectTarget, s_texUKnotVec );
  glUniform1i( s_uBsps.ukvTex, 0 );

  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( rectTarget, s_texUKnotTex );
  glUniform1i( s_uBsps.uknotTex2D, 1 );

  glActiveTexture( GL_TEXTURE2 );
  glBindTexture( rectTarget, s_texVKnotVec );
  glUniform1i( s_uBsps.vkvTex, 2 );

  glActiveTexture( GL_TEXTURE3 );
  glBindTexture( rectTarget, s_texVKnotTex );
  glUniform1i( s_uBsps.vknotTex2D, 3 );

  glActiveTexture( GL_TEXTURE4 );
  glBindTexture( rectTarget, s_texCp );
  glUniform1i( s_uBsps.cpTex, 4 );

  glActiveTexture( GL_TEXTURE5 );
  glBindTexture( GL_TEXTURE_2D, s_texLight );
  glUniform1i( s_uBsps.lightTex, 5 );
    
#ifndef USE_OLD

  glmeshvbo.setIsDrawWireframe( false );
  glmeshvbo.draw();
  glmeshvbo.setIsDrawWireframe( pwfflag );

#else // USE_OLD

  int udiv = 4;
  int vdiv = 8;
  for ( int i = 0; i < udiv; ++i )
    {
      for ( int j = 0; j < vdiv; ++j )
	{
	  int fid = i * vdiv + j;
	  std::vector<float>& ukv = bsps[fid].ukv();
	  float u0 = ukv[0];
	  float u1 = ukv[ukv.size()-1];

	  std::vector<float>& vkv = bsps[fid].vkv();
	  float v0 = vkv[0];
	  float v1 = vkv[vkv.size()-1];

	  float minx = -1.0f + 2.0f * (float) i / (float) udiv;
	  float maxx = -1.0f + 2.0f * (float) (i+1) / (float) udiv;
	  float miny = -1.0f + 2.0f * (float) j / (float) vdiv;
	  float maxy = -1.0f + 2.0f * (float) (j+1) / (float) vdiv;
	  
	  // use glTexCoord3f for fragment program
	  // glTexCoord3f( u, v, vertex/normal flag ) vertex ... -1, normal ... 1
	  glBegin( GL_QUADS );
	  glTexCoord3f( u0, v0, fid ); glVertex2f( minx, miny );
	  glTexCoord3f( u1, v0, fid ); glVertex2f( maxx, miny );
	  glTexCoord3f( u1, v1, fid ); glVertex2f( maxx, maxy );
	  glTexCoord3f( u0, v1, fid ); glVertex2f( minx, maxy );
	  glEnd();
	}
    }

#if 0
  glBegin( GL_QUADS );
  glTexCoord3f( u0, v0, 1 ); glVertex2f( -1.0f,  .0f );
  glTexCoord3f( u1, v0, 1 ); glVertex2f(  1.0f,  .0f );
  glTexCoord3f( u1, v1, 1 ); glVertex2f(  1.0f, 1.0f );
  glTexCoord3f( u0, v1, 1 ); glVertex2f( -1.0f, 1.0f );
  glEnd();
#endif

#endif

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( rectTarget, 0 );
  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( rectTarget, 0 );
  glActiveTexture( GL_TEXTURE2 );
  glBindTexture( rectTarget, 0 );
  glActiveTexture( GL_TEXTURE3 );
  glBindTexture( rectTarget, 0 );
  glActiveTexture( GL_TEXTURE4 );
  glBindTexture( rectTarget, 0 );
  glActiveTexture( GL_TEXTURE5 );
  glBindTexture( GL_TEXTURE_2D, 0 );
  glActiveTexture( GL_TEXTURE0 );

  glUseProgram( 0 );

#ifndef RENDER_TO_TEXTURE

#  ifdef USE_VBO
  rva->Read( GL_FRONT, pbuffer.GetWidth(), pbuffer.GetHeight() );
#  endif

#  ifdef USE_VAR
  var->Read( &pbuffer );
#  endif

#  ifdef USE_OLD
  std::vector<float> tmp( 3 * calc_width * calc_height );
  glReadPixels( 0, 0, pbuffer.GetWidth(), pbuffer.GetHeight(), GL_RGB, GL_FLOAT, &tmp[0] );
  pt.clear(); 
  for ( int i = 0; i < pbuffer_width*pbuffer_height; ++i )
    {
      Point3f p( tmp[3*i], tmp[3*i+1], tmp[3*i+2] );
      pt.push_back( p );
    }
#  endif // USE_OLD

#endif // RENDER_TO_TEXTURE


//   pbuffer->Deactivate();


#endif // USE_FRAG_PROGRAM

  ////////////////////////////////////////////////////////////////////////////////////
  //
  // 2nd pass
  //
  ////////////////////////////////////////////////////////////////////////////////////

#if 0

#ifdef RENDER_TO_TEXTURE

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  cgGLEnableProfile( fragProfile );
  cgGLBindProgram( rentexProgram );

  pbuffer->Bind(0);
  glBindTexture( GL_TEXTURE_RECTANGLE_NV, pbuffer->tex(0) );

  glBegin( GL_QUADS );
  glTexCoord2f(0, 0);                    glVertex2f(-1, -1);
  glTexCoord2f(0, calc_height);          glVertex2f(-1,  1);
  glTexCoord2f(calc_width, calc_height); glVertex2f( 1,  1);
  glTexCoord2f(calc_width, 0);           glVertex2f( 1, -1);
  glEnd();

  cgGLDisableProfile( fragProfile );

#else // RENDER_TO_TEXTURE

  pane.setIsGradientBackground( true );
  pane.clear( width, height );
  pane.setView();

#  ifndef USE_FRAG_PROGRAM
  pane.setLight();
#  endif

//  ::glEnable( GL_LIGHTING );
//   GLMaterial glm;
//   glm.bind();

//   glEnable( GL_VERTEX_PROGRAM_NV );
#  ifdef USE_VBO
  cgGLBindProgram( vertProgram );
  cgGLEnableProfile( vertProfile );
  
  cgGLSetStateMatrixParameter( cgGetNamedParameter(vertProgram, "ModelViewProj"),
			       CG_GL_MODELVIEW_PROJECTION_MATRIX,
			       CG_GL_MATRIX_IDENTITY );
#  endif

  glPointSize( 1.0f );
  glColor3f( 1.0f, 1.0f, 1.0f );

#  ifndef USE_FRAG_PROGRAM

//   glmeshl.draw();

#if 0
  ::glDisable( GL_LIGHTING );

  ::glBegin( GL_POINTS );
  int udiv = 1; int vdiv = 500;
  for ( int k = 0; k < bsps.size(); ++k )
    {
      for ( int i = 0; i <= udiv; ++i )
	{
	  float u = bsps[k].paramU( i, udiv );
	  for ( int j = 0; j <= vdiv; ++j )
	    {
	      float v = bsps[k].paramV( j, vdiv );
	      Point3f pt; Vector3f nm;
	      bsps[k].eval3( u, v, pt, nm );
	      ::glVertex3f( pt.x, pt.y, pt.z );
	    }
	}
    }
  ::glEnd();
#endif

#if 0
  for ( int i = 0; i < bsps.size(); ++i )
    {
      std::vector<Point3f>& cp = bsps[i].cp();

      for ( int j = 0; j < bsps[i].n_ucp(); ++j )
	{
	  ::glBegin( GL_LINE_STRIP );
	  for ( int k = 0; k < bsps[i].n_vcp(); ++k )
	    {
	      ::glVertex3f( cp[j * bsps[i].n_vcp()+k].x, 
			    cp[j * bsps[i].n_vcp()+k].y, 
			    cp[j * bsps[i].n_vcp()+k].z );
	    }
	  ::glEnd();
	}

      for ( int k = 0; k < bsps[i].n_vcp(); ++k )
	{
	  ::glBegin( GL_LINE_STRIP );
	  for ( int j = 0; j < bsps[i].n_ucp(); ++j )
	    {
	      ::glVertex3f( cp[j * bsps[i].n_vcp()+k].x, 
			    cp[j * bsps[i].n_vcp()+k].y, 
			    cp[j * bsps[i].n_vcp()+k].z );
	    }
	  ::glEnd();
	}
    }
  ::glEnable( GL_LIGHTING );
#  endif

#  else // USE_FRAG_PROGRAM

#    ifdef USE_OLD
  ::glDisable( GL_LIGHTING );
  glBegin( GL_POINTS );
  for ( int i = 0; i < pt.size(); ++i )
    {
//       glNormal3f( nm[i].x, nm[i].y, nm[i].z );
      glVertex3f( pt[i].x, pt[i].y, pt[i].z );
    }
  glEnd();
  ::glEnable( GL_LIGHTING );
#    endif // USE_OLD

#    ifdef USE_VBO
  rva->Draw( GL_POINTS );
#    endif

#    ifdef USE_VAR
  var->Draw( GL_POINTS );
#    endif

#    if USE_VBO
  cgGLDisableProfile( vertProfile );
//   glDisable( GL_VERTEX_PROGRAM_ARB );
#    endif

#  endif // USE_FRAG_PROGRAM

  ::glDisable( GL_LIGHTING );

#endif // RENDER_TO_TEXTURE

#endif

}

////////////////////////////////////////////////////////////////////////////////////

static bool s_glResourcesReady = false;

/* One-shot GL loader + mesh GPU setup (after a valid OpenGL context exists). */
static void initGLResourcesOnce()
{
  if ( s_glResourcesReady )
    return;

  MakeGlutWindowCurrent();

  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if ( err != GLEW_OK )
    std::cerr << "glewInit: " << glewGetErrorString( err ) << std::endl;

#if defined( _WIN32 )
  if ( WGLEW_EXT_swap_control )
    wglSwapIntervalEXT( 0 );
#endif

  initShaders();
  setTexture( bspstex );

  pane.initGL();
  pane.setIsGradientBackground( false );
  pane.setLightParameters( 0, myLight );

  glmeshvbo.setMesh( mesh );
  glmeshvbo.setIsSmoothShading( true );
  glmeshvbo.setMaterial( myMatl );
  glmeshvbo.setIsDrawWireframe( pwfflag );

  glmeshr.setMesh( mesh );
  glmeshr.setIsSmoothShading( false );
  glmeshr.setMaterial( myMatl );

  s_glResourcesReady = true;
}

static void reshape( int w, int h )
{
  MakeGlutWindowCurrent();

  if ( !s_glResourcesReady && w > 0 && h > 0 )
    initGLResourcesOnce();

  pane.reshape( w, h );
}

void display()
{
  if ( !s_glResourcesReady )
    initGLResourcesOnce();

  pane.clear( calc_width, calc_height );

  if ( (fGPUflag == DISPLAY_POLYGON) || (fGPUflag == DISPLAY_ISOPHOTO) )
    {
      displayPolygons();
    }
  else if ( (fGPUflag == DISPLAY_FRAGGPU) || (fGPUflag == DISPLAY_GPUISO) )
    {
      displayFragmentGPU();
    }

  pane.finish();

  checkGLErrors("display");

  ::glutSwapBuffers();

  // fps.frame();
  // if ( fps.timing_updated() )
  //   sprintf( buf,"%.3f fps", fps.get_fps() );
  double f = c11fps.CheckGetFPS();
  if ( max_c11fps < f ) max_c11fps = f;

  sprintf( txt, "B-spline Surfaces on GPU - %s - %.3f fps", typebuf, max_c11fps );
  ::glutSetWindowTitle( txt );

  ::glutReportErrors();

}

////////////////////////////////////////////////////////////////////////////////////

void mouse( int button, int state, int x, int y )
{
  pane.setScreenXY( x, y );

  // Rotate
  if ( state == GLUT_DOWN )
    {
      if ( button == GLUT_LEFT_BUTTON )        pane.startRotate();
      else if ( button == GLUT_MIDDLE_BUTTON ) pane.startZoom();
      else if ( button == GLUT_RIGHT_BUTTON )  pane.startMove();
      ::glutPostRedisplay();
    }
  else if ( state == GLUT_UP )
    {
      pane.finishRMZ();
      ::glutPostRedisplay();
    }
}

void motion( int x, int y )
{
  if ( pane.isRotate() )
    {
      pane.updateRotate( x, y );
      ::glutPostRedisplay();
    }
  if ( pane.isMove() )
    {
      pane.updateMove( x, y );
      ::glutPostRedisplay();
  }
  if ( pane.isZoom() )
    {
      pane.updateZoom( x, y );
      ::glutPostRedisplay();
    }
}

void keyboard( unsigned char c, int x, int y )
{
  VWIO vw_out;

  switch ( c )
    {
    case 'q':

#ifdef USE_VBO      
      delete rva;
#endif

#ifdef USE_VAR
      delete var;
#endif

      exit(0);
      break;

    case '1':

      fGPUflag = DISPLAY_POLYGON;
      sprintf( typebuf,"Polygon Shading" );
      max_c11fps = 0.0;

      break;

    case '2':

      fGPUflag = DISPLAY_ISOPHOTO;
      sprintf( typebuf,"Polygon Isophotos" );
      max_c11fps = 0.0;

      break;

    case '3':

      fGPUflag = DISPLAY_FRAGGPU;
      sprintf( typebuf,"GPU Shading" );
      max_c11fps = 0.0;

      break;

    case '4':

      fGPUflag = DISPLAY_GPUISO;
      sprintf( typebuf,"GPU Isophotos" );
      max_c11fps = 0.0;

      break;

    case 'e':

      pwfflag = (pwfflag == true) ? false : true;
      glmeshvbo.setIsDrawWireframe( pwfflag );


      break;

    case 'w':

      wfflag = (wfflag == true) ? false : true;
      
      break;

    case 'v':

      cout << "output to .vw file ... " << endl;
      vw_out.outputToFile( "tmp.vw", pane.manip() );
      cout << "done." << endl;
      
      break;

    case 'p':

      // screen capture
      //pane.setRecordPNG( true );
      //pane.setIsCaptureDepth( true );
      //pane.setPNGFile( "save_screen.png" );
      
      break;

    case '+':
    case '=':
      pane.setScreenXY( x, y );
      pane.updateWheelZoom( 1 );
      break;

    case '-':
    case '_':
      pane.setScreenXY( x, y );
      pane.updateWheelZoom( -1 );
      break;

    default:
      break;
      
    }
  
  ::glutPostRedisplay();
}

void idle( void ) 
{
  ::glutPostRedisplay();
}

int main( int argc, char **argv )
{
#if 1
  if ( argc < 2 )
    {
      std::cerr << "Usage: " << argv[0] << " in.3dm (or in.bsps) (in.vw)" << std::endl;
      exit(1);
    }
#endif

  Point3f center;
  float   maxlen;

#if 1
  ONX_Model model;
  DM3IO dm3io( model );
  dm3io.inputFromFile( argv[1], bsps );
  dm3io.normalizeCp( bsps, center, &maxlen );
#endif

#if 0
  BSPSIO bspsio;
  bspsio.inputFromFile( argv[1], bsps );
  bspsio.normalizeCp( bsps, center, &maxlen );
#endif

  bspstex.init( bsps );

#if 1
  cout << "polygonize ... ";
  for ( int i = 0; i < bsps.size(); ++i )
    {
      bsps[i].polygonize( mesh, 10, 10 );
    }
  cout << "done." << endl;
  mesh.printInfo();
#endif

#if 0
  POLIO polio( mesh );
  polio.inputFromFile( argv[2] );
  mesh.normalize();
#endif

#if 0
  SMFRIO smflio( mesh );
  smflio.outputToFile( "tmp.smf", true, true, false );
  exit(1);
#endif

#if 0
  SMFRIO smflio( mesh );
  smflio.inputFromFile( argv[2] );
#endif

#if 0
  for ( int i = 0; i <= 500; ++i )
    {
      float t = kv[0] + (kv[kv.size()-1] - kv[0]) * (float) i / 500;
      Point3f s;
      bspstex.evalPos3( t, s );
      pt.push_back( s );
    }
#endif

  /* freeglut requires glutInit() before glutInitWindowSize(); Apple GLUT tolerated the reverse. */
  ::glutInit( &argc, argv );
  ::glutInitWindowSize( width, height );
  //::glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL );
  ::glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
  ::glutCreateWindow( argv[0] );
  MakeGlutWindowCurrent();

  ::glutReshapeFunc( reshape );
  ::glutDisplayFunc( display );
  ::glutKeyboardFunc( keyboard );
  ::glutMouseFunc( mouse );
  ::glutMotionFunc( motion );
  ::glutIdleFunc( idle );

  pane.init( width, height );

  initFPBuffer();

#ifdef USE_VAR
  var = new RenderableVertexArray( pbuffer_width * pbuffer_height, 1 );
#endif

#ifdef USE_VBO
  rva = new RenderVertexArray( pbuffer_width * pbuffer_height, 3 );
#endif

  /* Display mode (GLEW / shaders / VBO init runs on first display — see initGLResourcesOnce). */
  fGPUflag = DISPLAY_FRAGGPU;
  sprintf( typebuf, "GPU Shading" );

  /* Establish viewport + run initGLResourcesOnce while the context is current (before first display). */
  reshape( width, height );

#if 0
  if ( argc == 3 )
    {
      VWIO vw_in;
      vw_in.inputFromFile( argv[2], pane.manip() );
    }
#endif

  ::glutMainLoop();
  return 0;
}
