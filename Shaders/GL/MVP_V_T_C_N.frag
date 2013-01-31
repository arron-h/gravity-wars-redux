uniform sampler2D   sTex;

varying vec2        vTexCoord;
varying vec4        vRGBA;

void main()
  {
  glFragColor = texture2D(sTex, vTexCoord) * vRGBA;
  }