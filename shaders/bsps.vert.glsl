////////////////////////////////////////////////////////////////////
// Pass-through vertex shader for B-spline fragment evaluation
// (replaces implicit fixed-function vertex path used with Cg).
////////////////////////////////////////////////////////////////////

#version 120

varying vec3 Access;

void main()
{
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  Access = gl_MultiTexCoord0.xyz;
}
