////////////////////////////////////////////////////////////////////
//
// $Id: bspsgpu.cxx 2021/06/01 01:50:09 kanai Exp $
//
// Copyright (c) 2004-2005 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#include "envDep.h"

#include "GL/glew.h"
#include "GL/wglew.h"

#include <GL/glut.h>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "mydef.h"
//#include "timer.hxx"
//#include "nvtimer.h"

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

CGcontext context;

CGprofile vertProfile;
CGprofile fragProfile;
CGprogram bspsProgram;
CGprogram isovProgram;
CGprogram isofProgram;

#ifdef USE_VBO
CGprogram vertProgram;
CGprofile vertProfile;
#endif

#ifdef RENDER_TO_TEXTURE
GLuint position;
CGprogram rentexProgram;
#endif

#ifdef USE_VBO
RenderVertexArray* rva = NULL;
#endif

#ifdef USE_VAR
RenderableVertexArray* var = NULL;
#endif

////////////////////////////////////////////////////////////////////////////////////

// テクスチャ格納のためのもとのデータ
std::vector<Point3f> cp;
std::vector<float> ukv;
std::vector<float> vkv;

#ifdef USE_OLD
// 表示用 points
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

static void handleCgError() 
{
  fprintf(stderr, "Cg error: %s\n", cgGetErrorString(cgGetError()));
}

void initCg()
{
  cgSetErrorCallback( handleCgError );
  context = cgCreateContext();

  // fragment program (1st-pass)
  fragProfile = cgGLGetLatestProfile( CG_GL_FRAGMENT );

  bspsProgram = cgCreateProgramFromFile( context, CG_SOURCE, "bsps.cg",
					 fragProfile, NULL, NULL );
  cgGLLoadProgram( bspsProgram );

  
  vertProfile = cgGLGetLatestProfile( CG_GL_VERTEX );

  // isoPhoto program for polygons
  isovProgram = cgCreateProgramFromFile( context, CG_SOURCE, "isophoto_vp.cg",
					 vertProfile, NULL, NULL );

  cgGLLoadProgram( isovProgram );

  isofProgram = cgCreateProgramFromFile( context, CG_SOURCE, "isophoto_fp.cg",
					 fragProfile, NULL, NULL );
  cgGLLoadProgram( isofProgram );
  

#ifdef USE_VBO
  // vertex program (2nd-pass)
  vertProfile = cgGLGetLatestProfile( CG_GL_VERTEX );
  vertProgram  = cgCreateProgramFromFile( context, CG_SOURCE, "vertex.cg",
 					  vertProfile, NULL, NULL );
  cgGLLoadProgram( vertProgram );
#endif

#ifdef RENDER_TO_TEXTURE
  // fragment program (2nd-pass)
  rentexProgram = cgCreateProgramFromFile( context, CG_SOURCE, "rentex.cg",
					   fragProfile, NULL, NULL );
  cgGLLoadProgram( rentexProgram );
#endif
}

void destroyCg()
{
  cgDestroyProgram( isovProgram );
  cgDestroyProgram( isofProgram );

  cgDestroyProgram( bspsProgram );
#ifdef RENDER_TO_TEXTURE
  cgDestroyProgram( rentexProgram );
#endif
#ifdef USE_VBO
  cgDestroyProgram( vertProgram );
#endif
  cgDestroyContext( context );
}

////////////////////////////////////////////////////////////////////////////////////

void setTexture( BSPSTexGPU& bspstex )
{
  // U Knot Vector
  GLuint UKnotVec;
  glGenTextures(1, &UKnotVec);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, UKnotVec);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  
  std::vector<float>& ukv = bspstex.ukv();
  glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA_NV, 
		bspstex.max_n_ukv(), bspstex.n_patch(), 
		0, GL_RGBA, GL_FLOAT, &ukv[0] );

  cgGLSetTextureParameter( cgGetNamedParameter(bspsProgram, "ukvTex"), UKnotVec );
  
  // U Knot Texture
  GLuint UKnotTex;
  glGenTextures(1, &UKnotTex);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, UKnotTex);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  std::vector<float>& ukt = bspstex.uknottex();
  glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA_NV, 
		bspstex.max_h_ukt() * bspstex.max_w_ukt(), bspstex.n_patch(), 
		0, GL_RGBA, GL_FLOAT, &ukt[0] );

  cgGLSetTextureParameter( cgGetNamedParameter(bspsProgram, "uknotTex2D"), UKnotTex );

  // V Knot Vector
  GLuint VKnotVec;
  glGenTextures(1, &VKnotVec);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, VKnotVec);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  
  std::vector<float>& vkv = bspstex.vkv();
  glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA_NV, 
		bspstex.max_n_vkv(), bspstex.n_patch(), 
		0, GL_RGBA, GL_FLOAT, &vkv[0] );

  cgGLSetTextureParameter( cgGetNamedParameter(bspsProgram, "vkvTex"), VKnotVec );
  
  // V Knot Texture
  GLuint VKnotTex;
  glGenTextures(1, &VKnotTex);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, VKnotTex);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  std::vector<float>& vkt = bspstex.vknottex();
  glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA_NV, 
		bspstex.max_h_vkt() * bspstex.max_w_vkt(), bspstex.n_patch(), 
		0, GL_RGBA, GL_FLOAT, &vkt[0] );

  cgGLSetTextureParameter( cgGetNamedParameter(bspsProgram, "vknotTex2D"), VKnotTex );

  // Control Points
  GLuint CpTex;
  glGenTextures(1, &CpTex);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, CpTex);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  
  std::vector<float>& cp = bspstex.cptex();
  glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA_NV, 
		bspstex.max_n_cp(), bspstex.n_patch(),
		0, GL_RGBA, GL_FLOAT, &cp[0] );

  cgGLSetTextureParameter( cgGetNamedParameter(bspsProgram, "cpTex"), CpTex );

  // Light Texture

  GLuint lightTex;
  glGenTextures( 1, &lightTex );
  glBindTexture( GL_TEXTURE_2D, lightTex );
  glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  
  PNGImage pngimage;
  std::vector<unsigned char> img;
  pngimage.inputFromFile("lattice.png", img );
  
  if ( !img.empty() )
    {
      GLint format;
      if ( pngimage.channel()==3) format = GL_RGB; else format = GL_RGBA;

      std::vector<float> fpng;

      for ( int i = 0; i < img.size(); ++i )
	{
	  float a =  (float) img[i] / 255.0;
	  fpng.push_back( a );
	}
      glTexImage2D( GL_TEXTURE_2D,
		    0, format,
		    pngimage.w(), pngimage.h(),
		    0, 
		    format,
		    GL_FLOAT,
		    &(fpng[0]) );

      cgGLSetTextureParameter( cgGetNamedParameter(bspsProgram, "lightTex"), lightTex );

    }
  else
    {
      cerr << "Error." << endl;
    }
}

////////////////////////////////////////////////////////////////////////////////////

void initFPBuffer()
{
  GLenum err = glewInit();
  if( err != GLEW_OK ) cout << "Error: %s" << glewGetErrorString(err) << endl;

  wglSwapIntervalEXT(0);

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
  static int glutWinId = glutGetWindow();

  glutSetWindow( glutWinId );
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

      cgGLEnableProfile( vertProfile );
      cgGLBindProgram( isovProgram );
      
      cgGLSetStateMatrixParameter( cgGetNamedParameter(isovProgram, "ModelViewProj"),
				   CG_GL_MODELVIEW_PROJECTION_MATRIX,
				   CG_GL_MATRIX_IDENTITY );

      cgGLEnableProfile( fragProfile );
      cgGLBindProgram( isofProgram );
      cgGLSetParameter3fv(cgGetNamedParameter( isofProgram, "lightPos"), current_light );
    }
  else
    {
      pane.setLight();
    }
  
  glmeshvbo.draw();
//   glmeshr.draw();

  if ( fGPUflag == DISPLAY_ISOPHOTO )
    {
      cgGLDisableProfile( fragProfile );
      cgGLDisableProfile( vertProfile );
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
  cgGLEnableProfile( fragProfile );
  cgGLBindProgram( bspsProgram );

  cgGLSetParameter3fv( cgGetNamedParameter(bspsProgram, "eyePos"), current_eye );
  cgGLSetParameter3fv( cgGetNamedParameter(bspsProgram, "lightPos"), current_light );
  cgGLSetParameter3fv( cgGetNamedParameter(bspsProgram, "ambientColor"), &(myMatl[0]) );
  cgGLSetParameter3fv( cgGetNamedParameter(bspsProgram, "diffuseColor"), &(myMatl[4]) );
  cgGLSetParameter3fv( cgGetNamedParameter(bspsProgram, "specularColor"), &(myMatl[12]) );
  cgGLSetParameter1f( cgGetNamedParameter(bspsProgram, "specularShininess"), myMatl[16] );
  cgGLSetParameter1f( cgGetNamedParameter(bspsProgram, "drawMode"), fGPUflag );

  cgGLEnableTextureParameter(cgGetNamedParameter(bspsProgram, "ukvTex"));
  cgGLEnableTextureParameter(cgGetNamedParameter(bspsProgram, "uknotTex2D"));
  cgGLEnableTextureParameter(cgGetNamedParameter(bspsProgram, "vkvTex"));
  cgGLEnableTextureParameter(cgGetNamedParameter(bspsProgram, "vknotTex2D"));
  cgGLEnableTextureParameter(cgGetNamedParameter(bspsProgram, "cpTex"));
  cgGLEnableTextureParameter(cgGetNamedParameter(bspsProgram, "lightTex"));

  cgGLSetStateMatrixParameter( cgGetNamedParameter(bspsProgram, "ModelViewProj"),
			       CG_GL_MODELVIEW_PROJECTION_MATRIX,
			       CG_GL_MATRIX_IDENTITY );
  cgGLSetStateMatrixParameter( cgGetNamedParameter(bspsProgram, "ModelViewIT"),
			       CG_GL_MODELVIEW_MATRIX,
			       CG_GL_MATRIX_INVERSE_TRANSPOSE );
    
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

  cgGLDisableTextureParameter(cgGetNamedParameter(bspsProgram, "ukvTex"));
  cgGLDisableTextureParameter(cgGetNamedParameter(bspsProgram, "uknotTex2D"));
  cgGLDisableTextureParameter(cgGetNamedParameter(bspsProgram, "vkvTex"));
  cgGLDisableTextureParameter(cgGetNamedParameter(bspsProgram, "vknotTex2D"));
  cgGLDisableTextureParameter(cgGetNamedParameter(bspsProgram, "cpTex"));
  cgGLDisableTextureParameter(cgGetNamedParameter(bspsProgram, "lightTex"));
  
  cgGLDisableProfile( fragProfile );

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

void display()
{
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

  ::glutInitWindowSize( width, height );
  ::glutInit( &argc, argv );
  //::glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL );
  ::glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
  ::glutCreateWindow( argv[0] );
  
  ::glutDisplayFunc( display );
  ::glutKeyboardFunc( keyboard );
  ::glutMouseFunc( mouse );
  ::glutMotionFunc( motion );
  ::glutIdleFunc( idle );
  
  pane.init( width, height );

  // Initialize float pbuffer
  initFPBuffer();
  initCg();
  setTexture( bspstex );

#ifdef USE_VAR
  var = new RenderableVertexArray( pbuffer_width * pbuffer_height, 1 );
#endif

#ifdef USE_VBO
  rva = new RenderVertexArray( pbuffer_width * pbuffer_height, 3 );
#endif

  // Initialize some state for the GLUT window's rendering context.
  MakeGlutWindowCurrent();
  pane.initGL();
  pane.setIsGradientBackground( false );
  pane.setLightParameters( 0, myLight );

  // Initialize display mode
  fGPUflag = DISPLAY_FRAGGPU;
  sprintf( typebuf, "GPU Shading" );

  glmeshvbo.setMesh( mesh );
  glmeshvbo.setIsSmoothShading( true );
  glmeshvbo.setMaterial( myMatl );
  glmeshvbo.setIsDrawWireframe( pwfflag );

  glmeshr.setMesh( mesh );
  glmeshr.setIsSmoothShading( false );
  glmeshr.setMaterial( myMatl );

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
