# Invoked from build tree with cwd = source tree:
#   cmake -E chdir <source_dir> cmake -DOUT=<abs_path> -P embed_shaders.cmake
if( NOT DEFINED OUT )
  message( FATAL_ERROR "embed_shaders.cmake requires -DOUT=" )
endif()
# Some generators pass stray quotes into -D values; strip them.
string( REGEX REPLACE "^[\"']+|[\"']+$" "" OUT "${OUT}" )

set( _d "EMBED_4029abc" )
file( READ "shaders/bsps.vert.glsl" _s )
file( WRITE "${OUT}"
  "// Generated from shaders/*.glsl - do not edit.\n"
  "#pragma once\n\n"
  "namespace bspsgpu_embedded {\n\n" )
file( APPEND "${OUT}"
  "static const char bsps_vert_glsl[] = R\"${_d}(${_s})${_d}\";\n\n" )
file( READ "shaders/bsps.frag.glsl" _s )
file( APPEND "${OUT}"
  "static const char bsps_frag_glsl[] = R\"${_d}(${_s})${_d}\";\n\n" )
file( READ "shaders/isophoto.vert.glsl" _s )
file( APPEND "${OUT}"
  "static const char isophoto_vert_glsl[] = R\"${_d}(${_s})${_d}\";\n\n" )
file( READ "shaders/isophoto.frag.glsl" _s )
file( APPEND "${OUT}"
  "static const char isophoto_frag_glsl[] = R\"${_d}(${_s})${_d}\";\n\n" )
file( APPEND "${OUT}" "} // namespace bspsgpu_embedded\n" )
