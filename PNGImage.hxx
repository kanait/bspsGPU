////////////////////////////////////////////////////////////////////
// Minimal PNG (and common formats) loader for the viewer using stb_image.
// Vendored header: external/stb/stb_image.h (https://github.com/nothings/stb)
////////////////////////////////////////////////////////////////////

#ifndef _PNGIMAGE_HXX
#define _PNGIMAGE_HXX 1

#include <vector>

#include "stb_image.h"

class PNGImage {

  int width_;
  int height_;
  int channel_;

public:
  PNGImage()
    : width_( 0 )
    , height_( 0 )
    , channel_( 0 )
  {
  }

  int w() const { return width_; }
  int h() const { return height_; }
  int channel() const { return channel_; }

  bool inputFromFile( const char* const path, std::vector<unsigned char>& img )
  {
    int w = 0, h = 0, n = 0;
    unsigned char* data = stbi_load( path, &w, &h, &n, 0 );
    if ( nullptr == data || w <= 0 || h <= 0 || n <= 0 )
      {
        width_ = height_ = channel_ = 0;
        img.clear();
        if ( data )
          stbi_image_free( data );
        return false;
      }

    width_ = w;
    height_ = h;
    channel_ = n;
    const size_t bytes = static_cast<size_t>( w ) * static_cast<size_t>( h ) * static_cast<size_t>( n );
    img.assign( data, data + bytes );
    stbi_image_free( data );
    return true;
  }
};

#endif // _PNGIMAGE_HXX
