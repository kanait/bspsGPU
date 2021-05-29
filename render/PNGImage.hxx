////////////////////////////////////////////////////////////////////
//
// $Id: PNGImage.hxx,v 1.1.1.1 2005/04/01 17:12:52 kanai Exp $
//
// Copyright (c) 2002 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _PNGIMAGE_HXX
#define _PNGIMAGE_HXX 1

#include "envDep.h"

#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

// requires libpng
#include "png.h"

#define CHAN_RGBA 4
#define CHAN_RGB  3

class PNGImage {

  int width_;
  int height_;

  unsigned char channel_;
  //    std::vector<std::vector<unsigned char> > buf_;
  unsigned char** buf_;
  
  bool isCaptureDepth_;

public:
  
  PNGImage() { buf_ = NULL; };
  PNGImage( int w, int h, bool depth_flag ) {
    setRes(w, h);
    isCaptureDepth_ = depth_flag;
    channel_ = ( isCaptureDepth_ == true ) ? CHAN_RGBA : CHAN_RGB;
    init();
  };
  PNGImage( int w, int h ) { 
    setRes(w, h);
    isCaptureDepth_ = false;
    channel_ = CHAN_RGB;
    init();
  };
  ~PNGImage() {
    free();
  };

  void init() {
    //     if ( buf_ ) return;
    buf_ = new unsigned char *[height_];
    for ( int i = 0; i < height_; ++i )
      {
        buf_[i] = new unsigned char [width_ * channel_];
      }
  };

  void free() {
    //     if ( !buf_ ) return;
    for ( int i = 0; i < height_; ++i )
      delete [] buf_[i];
    delete [] buf_;
    buf_ = NULL;
  };
    
  void setRes( int w, int h ) { setW(w); setH(h); };
  void setW( int w ) { width_ = w; };
  void setH( int h ) { height_ = h; };
  int w() const { return width_; };
  int h() const { return height_; };

  int channel() const { return channel_; };
  void setChannel( int channel ) { channel_ = channel; };

  bool capture_and_write( const char* filename ) {
    if ( !buf_ ) return false;

    std::vector<unsigned char> p( width_ * height_ * channel_ );
    // std::vector<unsigned char> d;

    // if ( isCaptureDepth_ )
    //   {
    //     d.resize( width_ * height_ );
    //   }

    capture( &p[0] );
    setPixels( &p[0] );

#if 0    
    if ( isCaptureDepth_ )
      {
        capture( &p[0], &d[0] );
        setPixels( &p[0], &d[0] );
      }
    else
      {
        capture( &p[0], NULL );
        setPixels( &p[0], NULL );
      }
#endif

    writeToFile( filename );

    std::cout << "save " << width_ << "x" << height_
              << " png image <" << filename << "> done. " << std::endl;
    return true;
  };

  void capture( unsigned char* p ) {

    ::glFlush();
    //   ::glReadBuffer( GL_BACK );

    if ( channel_ == 4 ) // RGB + depth
      {
        ::glReadPixels( 0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, p );
      }
    else if ( channel_ == 3 ) // RGB
      {
        ::glReadPixels( 0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, p );
      }
#if 0
    if ( isCaptureDepth_ )
      {
        ::glReadPixels( 0, 0, width_, height_,
                        GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, d );
      }
#endif
  };

  void setGtoRGB( unsigned char* g, unsigned char* rgb ) {
    int c = 0;
    for ( int i = 0; i < width_; ++i )
      {
        for ( int j = 0; j < height_; ++j )
          {
            int n = height_ * i + j;
            rgb[c++] = g[n];
            rgb[c++] = g[n];
            rgb[c++] = g[n];
          }
      }
  };

  void writeGrayImage( int width, int height, unsigned char* p, const char* pngfile ) {
    //   init();
    setRes( width, height );
    setChannel( CHAN_RGB );
    init();
    std::vector<unsigned char> q( width * height * CHAN_RGB );
    setGtoRGB( p, &q[0] );
    setPixels( &q[0] );
    writeToFile( pngfile );
    //   free();

    std::cout << "save " << width_ << "x" << height_
              << " png image <" << pngfile << "> done. " << std::endl;
  };

  void setPixels( unsigned char* p ) {
    for ( int i = 0; i < height_; ++i )
      {
        for ( int j = 0; j < width_; ++j )
          {
            int np = channel_ * (width_ * (height_ - 1 - i) + j);
            int kchan;
            if ( channel() <= CHAN_RGB ) kchan = channel_;
            else kchan = CHAN_RGB;
            for ( int k = 0; k < kchan; ++k )
              buf_[i][channel_*j+k]   = p[np+k];
            if ( isCaptureDepth_ == true )
              {
                buf_[i][channel_*j+3] = (p[np+3] == 255) ? 255 : 0;
              }
#if 0
            if ( (isCaptureDepth_ == true) && (d != NULL) )
              {
                int nd = (width_ * (height_ - 1 - i) + j);
                buf_[i][channel_*j+3] = (d[nd] == 255) ? 0 : 255;
                // if ( d[nd] != 255 ) cout << d[nd] << endl;
              }
#endif
          }
      }
  };

  void writeToFile( const char* pngfile ) {
    FILE* fp;
    if ( (fp = fopen(pngfile, "wb")) == NULL ) return;

    assert(fp);

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  NULL, NULL, NULL);
    assert(png_ptr);

    png_infop info_ptr = png_create_info_struct(png_ptr);
    assert(info_ptr);

    png_init_io(png_ptr, fp);

    png_byte ct;
    //   if ( channel() == CHAN_RGBA ) ct = PNG_COLOR_TYPE_RGB_ALPHA;
    //   else if (  channel() == CHAN_RGB ) ct = PNG_COLOR_TYPE_RGB;
    //   else ct = PNG_COLOR_TYPE_GRAY;
    if ( isCaptureDepth_ ) ct = PNG_COLOR_TYPE_RGB_ALPHA;
    else ct = PNG_COLOR_TYPE_RGB;
    png_set_IHDR( png_ptr, info_ptr, width_, height_, 8,
                  ct, 
                  PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, 
                  PNG_FILTER_TYPE_DEFAULT );
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, buf_);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose( fp );
  };

  bool inputFromFile( const char* const pngfile, std::vector<unsigned char>& img ) {
    if ( inputFromFile( pngfile ) == false ) return false;

    img.resize( width_*height_*channel_ );
    for ( int i = 0; i < height_; ++i )
      {
        for ( int j = 0; j < width_; ++j )
          {
            int np = channel_ * (width_ * (height_ - 1 - i) + j);
            for ( int k = 0; k < channel_; ++k )
              img[np+k] = buf_[i][channel_*j+k];
          }
      }
    return true;
  };

  bool inputFromFile( const char* const pngfile ) {
    // open the PNG input file
    if (!pngfile) return false;

    FILE* fp;
    if ( (fp = fopen(pngfile, "rb")) == NULL ) return false;

    // first check the eight byte PNG signature
    png_byte sig[8];
    fread(sig, 1, 8, fp);
    //if (!png_check_sig(sig, 8))
    if (png_sig_cmp(sig, 0, 8))
      { 
        if(fp) fclose(fp); 
        return false; 
      }

    // start back here!!!!

    // create the two png(-info) structures

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png_ptr)
      { 
        if(fp) fclose(fp); 
        return false; 
      }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
      {
        png_destroy_read_struct(&png_ptr, 0, 0);
        if(fp) fclose(fp); 
      }

    // initialize the png structure
    png_init_io(png_ptr, fp);

    png_set_sig_bytes(png_ptr, 8);

    // read all PNG info up to image data
    png_read_info(png_ptr, info_ptr);

    // get width, height, bit-depth and color-type
    //unsigned long w, h;
    png_uint_32 w, h;
    int bit_depth, color_type;
    png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);
    //   width_ = w; height_ = h;

    // expand images of all color-type and bit-depth to 3x8 bit RGB images
    // let the library process things like alpha, transparency, background

    if (bit_depth == 16) png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand(png_ptr);
    if (bit_depth < 8) png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_expand(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_gray_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
      png_set_strip_alpha(png_ptr);

    // if required set gamma conversion
    double gamma;
    if ( png_get_gAMA(png_ptr, info_ptr, &gamma) )
      png_set_gamma(png_ptr, (double) 2.2, gamma);

    // after the transformations have been registered update info_ptr data
    png_read_update_info(png_ptr, info_ptr);

    // get again width, height and the new bit-depth and color-type
    png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);

    width_ = w; height_ = h;


    // row_bytes is the width x number of channels
    //   png_uint_32 channels, row_bytes;
    png_uint_32 row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    png_uint_32 channels = png_get_channels(png_ptr, info_ptr);

    channel_ = channels;

    std::cout << "w " << width_
              << " h " << height_
              << " chan " << channel_
              << std::endl;

    // now we can allocate memory to store the image

    png_byte * img = new png_byte[row_bytes * h];

    // and allocate memory for an array of row-pointers

    png_byte ** row = new png_byte * [h];

    // set the individual row-pointers to point at the correct offsets

    for (unsigned int i = 0; i < h; i++)
      row[i] = img + i * row_bytes;

    // now we can go ahead and just read the whole image

    png_read_image(png_ptr, row);

    // read the additional chunks in the PNG file (not really needed)

    png_read_end(png_ptr, NULL);

    init();
    //       image = array2<vec3ub>(w, h);

    {
      for( unsigned int i=0; i < w; ++i )
        for( unsigned int j=0; j < h; ++j )
          for ( unsigned int k=0; k < channel_; ++k )
            {
              buf_[i][channel_*j+k] = *( img + ((h-j-1)*row_bytes + i * channel_ + k) );
              //{ image(i,j) = vec3ub(img + ((h-j-1)*row_bytes + i * 3)); }
            }
    }

    delete [] row;
    delete [] img;

    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    if(fp) fclose (fp);

    return true;
  };

};

#endif // _PNGIMAGE_HXX
