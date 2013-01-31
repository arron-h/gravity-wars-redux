uniform sampler2D   sTex;

varying mediump vec2        vTexCoord;
varying mediump vec4        vRGBA;

void main()
  {
  gl_FragColor = texture2D(sTex, vTexCoord) * vRGBA;
  }