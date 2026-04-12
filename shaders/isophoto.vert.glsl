////////////////////////////////////////////////////////////////////
// Isophoto display (from isophoto_vp.cg)
////////////////////////////////////////////////////////////////////

#version 120

varying vec4 ppos;
varying vec4 pnrm;

void main()
{
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  ppos = gl_Vertex;
  pnrm = vec4( gl_Normal.xyz, 0.0 );
}
