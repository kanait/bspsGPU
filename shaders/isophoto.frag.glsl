////////////////////////////////////////////////////////////////////
// Isophoto fragment (from isophoto_fp.cg)
////////////////////////////////////////////////////////////////////

#version 120

uniform vec3 lightPos;

varying vec4 ppos;
varying vec4 pnrm;

void main()
{
  vec3 lv = lightPos - ppos.xyz;
  vec3 lvn = normalize( lv );
  vec3 nrm3 = pnrm.xyz;

  float d = dot( nrm3, lvn ) * 10.0;

  vec4 color = vec4( 0.0, 0.0, 0.0, 1.0 );

  /* GLSL 1.20 has no integer bitwise ops; use float mod for parity */
  float band = floor( d );
  if ( mod( band, 2.0 ) < 1.0 )
    color += vec4( 0.1, 0.1, 0.1, 1.0 );
  else
    color += vec4( 0.9, 0.9, 0.9, 1.0 );

  gl_FragColor = color;
}
