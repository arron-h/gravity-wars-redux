uniform sampler2D   sTex;

varying mediump vec2        vTexCoord;
varying lowp vec4        vRGBA;

void main()
  {
  glFragColor = texture2D(sTex, vTexCoord) * vRGBA;
  }