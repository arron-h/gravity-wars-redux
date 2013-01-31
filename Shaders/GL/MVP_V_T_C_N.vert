attribute vec3    inVertex;
attribute vec2    inTexCoord;
attribute vec4    inRGBA;
attribute vec3    inNormal;

uniform mat4      mxMVP;
uniform vec3      vLightDir;

varying vec2      vTexCoord;
varying vec4      vRGBA;

void main()
  {
  gl_Position = mxMVP * vec4(inVertex, 1.0);
  
  vTexCoord = inTexCoord;
  
  // Perform some quick Per-Vertex lighting.
  float vNDotL    = max(dot(inNormal, vLightDir), 0.0);
  vec3  vColComp  = inRGBA.rgb * vNDotL;
  vRGBA           = vec4(vColComp, inRGBA.a);
  }