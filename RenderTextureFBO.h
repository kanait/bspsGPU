//
// simple wrapper to encapsulate a framebuffer object using render-to-texture and 
// its associated texture object
//
// by Takashi Kanai
//
// v1.0 - initial release
// v1.1 - added texFormat_, texType_ attibutes in glTexImage2D to create textures

#ifndef RENDER_TEXTURE_FBO_H
#define RENDER_TEXTURE_FBO_H

#define MAX_BUFFERS  4

const GLenum colorAttachment_[] = {
  GL_COLOR_ATTACHMENT0_EXT,
  GL_COLOR_ATTACHMENT1_EXT,
  GL_COLOR_ATTACHMENT2_EXT,
  GL_COLOR_ATTACHMENT3_EXT
};
  
class RenderTextureFBO {

public:

  RenderTextureFBO( int texWidth, int texHeight, GLenum texTarget, GLenum texInternalFormat,
		    GLenum texFormat, GLenum texType, int nDrawBuffers )
    : texWidth_(texWidth), texHeight_(texHeight), 
      texTarget_(texTarget), texInternalFormat_(texInternalFormat),
      texFormat_(texFormat), texType_(texType),
      nDrawBuffers_(nDrawBuffers)
  {
    filterMode_ = ( (texTarget == GL_TEXTURE_RECTANGLE_NV) ||
		    (texTarget == GL_TEXTURE_RECTANGLE_EXT) ) ? GL_NEAREST : GL_LINEAR;
//     maxCoordS_  = texWidth;
//     maxCoordT_  = texHeight;
    maxCoordS_  = ( (texTarget == GL_TEXTURE_RECTANGLE_NV) ||
		    (texTarget == GL_TEXTURE_RECTANGLE_EXT) ) ? texWidth : 1.0f;
    maxCoordT_  = ( (texTarget == GL_TEXTURE_RECTANGLE_NV) ||
		    (texTarget == GL_TEXTURE_RECTANGLE_EXT) ) ? texHeight : 1.0f;

    glGenFramebuffersEXT( 1, &fb_ );
    glGenRenderbuffersEXT( 1, &depth_rb_ );

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fb_ );

    // init textures
    buffers_.resize( nDrawBuffers );
    for ( int i = 0; i < nDrawBuffers; ++i )
      {
	createTexture( i );
	buffers_[i] = colorAttachment_[i];
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, buffers_[i], texTarget_, tex_[i], 0 );
      }

    // initialize depth renderbuffer
    glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, depth_rb_ );
    glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, texWidth, texHeight );
    glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, 
				  GL_RENDERBUFFER_EXT, depth_rb_ );

    checkFramebufferStatus();

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
  };

  ~RenderTextureFBO()
  {
    for ( int i = 0; i < nDrawBuffers_; ++i )
      glDeleteTextures( 1, &(tex_[i]) );
    glDeleteRenderbuffersEXT( 1, &depth_rb_ );
    glDeleteFramebuffersEXT( 1, &fb_ );
  };

  void Activate() { 
    glBindTexture( texTarget_, 0 );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fb_ );
  };

  void Deactivate() { 
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
  };

  void Read( int i = 0 ) {
    glReadBuffer( buffers_[i] );
  };

  GLint tex( int i = 0 ) const { return tex_[i]; };

  void Draw( int i = 0 ) {
    glDrawBuffer( buffers_[i] );
  };

  void Bind( int i = 0 ) {
    glBindTexture( texTarget_, tex_[i] );
    glEnable( texTarget_ );  
  };
  
  void Release( int i = 0 ) {
    glBindTexture( texTarget_, tex_[i] );
    glDisable( texTarget_ );  
  };

  inline int GetWidth() { return texWidth_; }
  inline int GetHeight() { return texHeight_; }

  void drawQuad() {

    glBegin( GL_QUADS );
    {
      glTexCoord2f( .0f,        .0f );        glVertex2f( -1.0f, -1.0f );
      glTexCoord2f( maxCoordS_, .0f );        glVertex2f(  1.0f, -1.0f );
      glTexCoord2f( maxCoordS_, maxCoordT_ ); glVertex2f(  1.0f,  1.0f );
      glTexCoord2f( .0f,        maxCoordT_ ); glVertex2f( -1.0f,  1.0f );
    }
    glEnd();

  };

  void checkFramebufferStatus()
  {
    GLenum status;
    status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status) {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
      printf("Unsupported framebuffer format\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
      printf("Framebuffer incomplete, missing attachment\n");
      break;
//    case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
//      printf("Framebuffer incomplete, duplicate attachment\n");
//      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
      printf("Framebuffer incomplete, attached images must have same dimensions\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
      printf("Framebuffer incomplete, attached images must have same format\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
      printf("Framebuffer incomplete, missing draw buffer\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
      printf("Framebuffer incomplete, missing read buffer\n");
      break;
    default:
      assert(0);
    }
  };

private:

  GLint  texWidth_;
  GLint  texHeight_;

  GLuint fb_;  // color render target
  GLuint depth_rb_; // depth render target
  GLuint tex_[MAX_BUFFERS]; // texture

  GLenum texTarget_;
  GLenum texInternalFormat_;
  GLenum texFormat_;
  GLenum texType_;

  // for multiple draw buffers
  std::vector<GLenum> buffers_;

  GLenum filterMode_;
  int maxCoordS_;
  int maxCoordT_;

  int nDrawBuffers_;

  void createTexture( int i ) {
    glGenTextures( 1, &(tex_[i]) );
    glBindTexture( texTarget_, tex_[i] );
    glTexParameterf( texTarget_, GL_TEXTURE_MIN_FILTER, filterMode_ );
    glTexParameterf( texTarget_, GL_TEXTURE_MAG_FILTER, filterMode_ );
    glTexParameterf( texTarget_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf( texTarget_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D( texTarget_, 0, texInternalFormat_, texWidth_, texHeight_, 0, 
		  texFormat_, texType_, NULL );
  };
};

#endif // RENDER_TEXTURE_FBO_H
