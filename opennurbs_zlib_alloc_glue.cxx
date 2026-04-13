////////////////////////////////////////////////////////////////////
// Bundled OpenNURBS zlib (C) calls zcalloc / zcfree by C ABI name.
// In the submodule, opennurbs_zlib_memory.cpp defines them in a C++
// translation unit without extern "C", so on ELF the symbols do not
// satisfy the C references from deflate.c / inflate.c. Provide C
// wrappers here and forward to oncalloc / onfree.
////////////////////////////////////////////////////////////////////

#include "opennurbs.h"

extern "C" {

void *zcalloc( void *opaque, unsigned items, unsigned size )
{
  (void)opaque;
  return oncalloc( static_cast<size_t>( items ), static_cast<size_t>( size ) );
}

void zcfree( void *opaque, void *ptr )
{
  (void)opaque;
  onfree( ptr );
}

} // extern "C"
