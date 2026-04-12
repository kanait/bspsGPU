////////////////////////////////////////////////////////////////////
//
// Minimal GLSL compile/link helpers (replaces NVIDIA Cg runtime).
//
////////////////////////////////////////////////////////////////////

#ifndef _GLSHADER_HXX
#define _GLSHADER_HXX 1

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "GL/glew.h"

inline bool readShaderFile( const char* path, std::string& out )
{
  std::ifstream f( path, std::ios::binary );
  if ( !f )
    return false;
  std::ostringstream buf;
  buf << f.rdbuf();
  out = buf.str();
  return !out.empty();
}

inline void printShaderInfoLog( GLuint obj, const char* stage, const char* path )
{
  GLint len = 0;
  glGetShaderiv( obj, GL_INFO_LOG_LENGTH, &len );
  if ( len <= 1 )
    return;
  std::vector<char> log( (size_t)len );
  glGetShaderInfoLog( obj, len, NULL, &log[0] );
  std::cerr << stage << " shader log (" << path << "):\n" << &log[0] << std::endl;
}

inline GLuint compileShader( GLenum type, const char* src, const char* path )
{
  GLuint s = glCreateShader( type );
  glShaderSource( s, 1, &src, NULL );
  glCompileShader( s );
  GLint ok = 0;
  glGetShaderiv( s, GL_COMPILE_STATUS, &ok );
  if ( !ok )
    {
      printShaderInfoLog( s, ( type == GL_VERTEX_SHADER ) ? "vertex" : "fragment", path );
      glDeleteShader( s );
      return 0;
    }
  return s;
}

inline void printProgramInfoLog( GLuint prog )
{
  GLint len = 0;
  glGetProgramiv( prog, GL_INFO_LOG_LENGTH, &len );
  if ( len <= 1 )
    return;
  std::vector<char> log( (size_t)len );
  glGetProgramInfoLog( prog, len, NULL, &log[0] );
  std::cerr << "program log:\n" << &log[0] << std::endl;
}

inline GLuint linkProgram( GLuint vs, GLuint fs )
{
  GLuint p = glCreateProgram();
  if ( vs ) glAttachShader( p, vs );
  if ( fs ) glAttachShader( p, fs );
  glLinkProgram( p );
  GLint ok = 0;
  glGetProgramiv( p, GL_LINK_STATUS, &ok );
  if ( !ok )
    {
      printProgramInfoLog( p );
      glDeleteProgram( p );
      p = 0;
    }
  if ( vs ) { glDetachShader( p, vs ); glDeleteShader( vs ); }
  if ( fs ) { glDetachShader( p, fs ); glDeleteShader( fs ); }
  return p;
}

/** @param vertLabel / fragLabel appear in compile logs only (e.g. embedded GLSL names). */
inline GLuint createProgramFromSources( const char* vertSrc, const char* vertLabel,
                                        const char* fragSrc, const char* fragLabel )
{
  GLuint vs = compileShader( GL_VERTEX_SHADER, vertSrc, vertLabel );
  if ( !vs ) return 0;
  GLuint fs = compileShader( GL_FRAGMENT_SHADER, fragSrc, fragLabel );
  if ( !fs )
    {
      glDeleteShader( vs );
      return 0;
    }
  return linkProgram( vs, fs );
}

inline GLuint createProgramFromFiles( const char* vertPath, const char* fragPath )
{
  std::string vsrc, fsrc;
  if ( !readShaderFile( vertPath, vsrc ) )
    {
      std::cerr << "Cannot read vertex shader: " << vertPath << std::endl;
      return 0;
    }
  if ( !readShaderFile( fragPath, fsrc ) )
    {
      std::cerr << "Cannot read fragment shader: " << fragPath << std::endl;
      return 0;
    }
  GLuint vs = compileShader( GL_VERTEX_SHADER, vsrc.c_str(), vertPath );
  if ( !vs ) return 0;
  GLuint fs = compileShader( GL_FRAGMENT_SHADER, fsrc.c_str(), fragPath );
  if ( !fs )
    {
      glDeleteShader( vs );
      return 0;
    }
  return linkProgram( vs, fs );
}

#endif // _GLSHADER_HXX
