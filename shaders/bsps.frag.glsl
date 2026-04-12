////////////////////////////////////////////////////////////////////
// B-spline surface fragment evaluation (from bsps.cg + binsearch.cg + bsps3.cg)
////////////////////////////////////////////////////////////////////

#version 120
#extension GL_ARB_texture_rectangle : require

// Match mydefine.h
const float FID_DIV = 10000.0;
const float DISPLAY_FRAGGPU = 2.0;
const float DISPLAY_GPUISO = 3.0;
const float BSP_CUBIC = 3.0;
const float BSP_QUINTIC = 5.0;

const int LOGN = 6;

uniform vec3 eyePos;
uniform vec3 lightPos;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float specularShininess;
uniform float drawMode;

uniform sampler2DRect ukvTex;
uniform sampler2DRect uknotTex2D;
uniform sampler2DRect vkvTex;
uniform sampler2DRect vknotTex2D;
uniform sampler2DRect cpTex;
uniform sampler2D lightTex;

varying vec3 Access;

int binarySearch( float t, float fid, float deg, sampler2DRect sortlist )
{
  vec4 a = texture2DRect( sortlist, vec2( 0.0, fid ) );
  float m = a.y - 1.0;

  float t0 = texture2DRect( sortlist, vec2( m - deg, fid ) ).x;
  int flag = 0;
  if ( abs( t - t0 ) < 1.0e-05 )
    flag = 1;

  int low = int( deg );
  int high = int( m - deg );

  int mid = ( low + high ) / 2;
  for ( int iter = 0; iter < LOGN; ++iter )
    {
      float ts = texture2DRect( sortlist, vec2( float( mid ), fid ) ).x;
      float te = texture2DRect( sortlist, vec2( float( mid + 1 ), fid ) ).x;
      if ( t < ts || t >= te )
        {
          if ( t < ts )
            high = mid;
          else
            low = mid;
          mid = ( low + high ) / 2;
        }
    }

  if ( flag == 1 )
    mid = int( m - ( deg + 1.0 ) );

  return mid;
}

void basisFuncDeriv3( float t, vec4 knot0, vec4 knot1, vec4 kd0, vec4 kd1,
                      out vec4 outN, out vec4 outNt )
{
  float N[6];

  N[0] = 1.0;

  N[1] = ( knot1.x - t ) * N[0] / kd0.x;
  N[2] = ( t - knot0.z ) * N[0] / kd0.x;

  N[3] = ( knot1.x - t ) * N[1] / kd0.y;
  N[4] = ( t - knot0.y ) * N[1] / kd0.y + ( knot1.y - t ) * N[2] / kd0.z;
  N[5] = ( t - knot0.z ) * N[2] / kd0.z;

  outN.x = ( knot1.x - t ) * N[3] / kd1.x;
  outN.y = ( t - knot0.x ) * N[3] / kd1.x + ( knot1.y - t ) * N[4] / kd1.y;
  outN.z = ( t - knot0.y ) * N[4] / kd1.y + ( knot1.z - t ) * N[5] / kd1.z;
  outN.w = ( t - knot0.z ) * N[5] / kd1.z;

  outNt.x = -3.0 * N[3] / kd1.x;
  outNt.y = 3.0 * N[3] / kd1.x - 3.0 * N[4] / kd1.y;
  outNt.z = 3.0 * N[4] / kd1.y - 3.0 * N[5] / kd1.z;
  outNt.w = 3.0 * N[5] / kd1.z;
}

void eval3( float u, float v, float fid,
            sampler2DRect ukvTex_, sampler2DRect uknotTex2D_,
            sampler2DRect vkvTex_, sampler2DRect vknotTex2D_,
            sampler2DRect cpTex_,
            out vec4 pos, out vec4 nrm )
{
  int uid = binarySearch( u, fid, BSP_CUBIC, ukvTex_ );
  float iu = float( uid - 3 );

  float ub = 4.0 * iu;
  vec4 uknot0 = texture2DRect( uknotTex2D_, vec2( ub, fid ) );
  vec4 uknot1 = texture2DRect( uknotTex2D_, vec2( ub + 1.0, fid ) );
  vec4 ukd0 = texture2DRect( uknotTex2D_, vec2( ub + 2.0, fid ) );
  vec4 ukd1 = texture2DRect( uknotTex2D_, vec2( ub + 3.0, fid ) );

  vec4 Nu, dNu;
  basisFuncDeriv3( u, uknot0, uknot1, ukd0, ukd1, Nu, dNu );

  int vid = binarySearch( v, fid, BSP_CUBIC, vkvTex_ );
  float iv = float( vid - 3 );

  float vb = 4.0 * iv;
  vec4 vknot0 = texture2DRect( vknotTex2D_, vec2( vb, fid ) );
  vec4 vknot1 = texture2DRect( vknotTex2D_, vec2( vb + 1.0, fid ) );
  vec4 vkd0 = texture2DRect( vknotTex2D_, vec2( vb + 2.0, fid ) );
  vec4 vkd1 = texture2DRect( vknotTex2D_, vec2( vb + 3.0, fid ) );

  vec4 Nv, dNv;
  basisFuncDeriv3( v, vknot0, vknot1, vkd0, vkd1, Nv, dNv );

  vec4 cc = texture2DRect( cpTex_, vec2( 0.0, fid ) );

  vec4 Cp[16];
  Cp[0] = texture2DRect( cpTex_, vec2( ( iu ) * cc.w + ( iv ) + 1.0, fid ) );
  Cp[1] = texture2DRect( cpTex_, vec2( ( iu + 1.0 ) * cc.w + ( iv ) + 1.0, fid ) );
  Cp[2] = texture2DRect( cpTex_, vec2( ( iu + 2.0 ) * cc.w + ( iv ) + 1.0, fid ) );
  Cp[3] = texture2DRect( cpTex_, vec2( ( iu + 3.0 ) * cc.w + ( iv ) + 1.0, fid ) );

  Cp[4] = texture2DRect( cpTex_, vec2( ( iu ) * cc.w + ( iv + 1.0 ) + 1.0, fid ) );
  Cp[5] = texture2DRect( cpTex_, vec2( ( iu + 1.0 ) * cc.w + ( iv + 1.0 ) + 1.0, fid ) );
  Cp[6] = texture2DRect( cpTex_, vec2( ( iu + 2.0 ) * cc.w + ( iv + 1.0 ) + 1.0, fid ) );
  Cp[7] = texture2DRect( cpTex_, vec2( ( iu + 3.0 ) * cc.w + ( iv + 1.0 ) + 1.0, fid ) );

  Cp[8] = texture2DRect( cpTex_, vec2( ( iu ) * cc.w + ( iv + 2.0 ) + 1.0, fid ) );
  Cp[9] = texture2DRect( cpTex_, vec2( ( iu + 1.0 ) * cc.w + ( iv + 2.0 ) + 1.0, fid ) );
  Cp[10] = texture2DRect( cpTex_, vec2( ( iu + 2.0 ) * cc.w + ( iv + 2.0 ) + 1.0, fid ) );
  Cp[11] = texture2DRect( cpTex_, vec2( ( iu + 3.0 ) * cc.w + ( iv + 2.0 ) + 1.0, fid ) );

  Cp[12] = texture2DRect( cpTex_, vec2( ( iu ) * cc.w + ( iv + 3.0 ) + 1.0, fid ) );
  Cp[13] = texture2DRect( cpTex_, vec2( ( iu + 1.0 ) * cc.w + ( iv + 3.0 ) + 1.0, fid ) );
  Cp[14] = texture2DRect( cpTex_, vec2( ( iu + 2.0 ) * cc.w + ( iv + 3.0 ) + 1.0, fid ) );
  Cp[15] = texture2DRect( cpTex_, vec2( ( iu + 3.0 ) * cc.w + ( iv + 3.0 ) + 1.0, fid ) );

  pos = ( Nu.x * Nv.x * Cp[0] + Nu.y * Nv.x * Cp[1] + Nu.z * Nv.x * Cp[2] + Nu.w * Nv.x * Cp[3] +
          Nu.x * Nv.y * Cp[4] + Nu.y * Nv.y * Cp[5] + Nu.z * Nv.y * Cp[6] + Nu.w * Nv.y * Cp[7] +
          Nu.x * Nv.z * Cp[8] + Nu.y * Nv.z * Cp[9] + Nu.z * Nv.z * Cp[10] + Nu.w * Nv.z * Cp[11] +
          Nu.x * Nv.w * Cp[12] + Nu.y * Nv.w * Cp[13] + Nu.z * Nv.w * Cp[14] + Nu.w * Nv.w * Cp[15] );

  vec4 pu = ( dNu.x * Nv.x * Cp[0] + dNu.y * Nv.x * Cp[1] + dNu.z * Nv.x * Cp[2] + dNu.w * Nv.x * Cp[3] +
             dNu.x * Nv.y * Cp[4] + dNu.y * Nv.y * Cp[5] + dNu.z * Nv.y * Cp[6] + dNu.w * Nv.y * Cp[7] +
             dNu.x * Nv.z * Cp[8] + dNu.y * Nv.z * Cp[9] + dNu.z * Nv.z * Cp[10] + dNu.w * Nv.z * Cp[11] +
             dNu.x * Nv.w * Cp[12] + dNu.y * Nv.w * Cp[13] + dNu.z * Nv.w * Cp[14] + dNu.w * Nv.w * Cp[15] );

  vec4 pv = ( Nu.x * dNv.x * Cp[0] + Nu.y * dNv.x * Cp[1] + Nu.z * dNv.x * Cp[2] + Nu.w * dNv.x * Cp[3] +
              Nu.x * dNv.y * Cp[4] + Nu.y * dNv.y * Cp[5] + Nu.z * dNv.y * Cp[6] + Nu.w * dNv.y * Cp[7] +
              Nu.x * dNv.z * Cp[8] + Nu.y * dNv.z * Cp[9] + Nu.z * dNv.z * Cp[10] + Nu.w * dNv.z * Cp[11] +
              Nu.x * dNv.w * Cp[12] + Nu.y * dNv.w * Cp[13] + Nu.z * dNv.w * Cp[14] + Nu.w * dNv.w * Cp[15] );

  nrm = vec4( normalize( cross( pu.xyz, pv.xyz ) ), 1.0 );
}

void main()
{
  float u = Access.x;
  float v = Access.y;
  float fid = floor( Access.z * FID_DIV + 0.5 );

  vec4 cc = texture2DRect( cpTex, vec2( 0.0, fid ) );

  vec4 pos;
  vec4 nrm;

  if ( cc.x == BSP_CUBIC )
    eval3( u, v, fid, ukvTex, uknotTex2D, vkvTex, vknotTex2D, cpTex, pos, nrm );

  vec4 color = vec4( 0.0, 0.0, 0.0, 1.0 );

  if ( abs( drawMode - DISPLAY_FRAGGPU ) < 0.001 )
    {
      vec3 nrm3 = nrm.xyz;
      vec3 lp = normalize( lightPos );
      vec3 halfVec = normalize( lp + eyePos );
      vec3 diffuse = max( dot( nrm3, lp ), 0.0 ) * diffuseColor;
      vec3 specular = vec3( max( dot( nrm3, halfVec ), 0.0 ) );
      specular = pow( specular.x, specularShininess ) * specularColor;
      color.xyz += ( ambientColor + diffuse + specular );
    }
  else if ( abs( drawMode - DISPLAY_GPUISO ) < 0.001 )
    {
      vec3 lv = lightPos - pos.xyz;
      vec3 lvn = normalize( lv );
      vec3 nrm3 = nrm.xyz;

      float d = dot( nrm3, lvn ) * 10.0;
      /* GLSL 1.20 has no integer bitwise ops; use float mod for parity */
      float band = floor( d );
      if ( mod( band, 2.0 ) < 1.0 )
        color += vec4( 0.1, 0.1, 0.1, 1.0 );
      else
        color += vec4( 0.9, 0.9, 0.9, 1.0 );
    }

  gl_FragColor = color;
}
