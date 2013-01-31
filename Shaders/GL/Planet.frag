uniform sampler2D   sTex;

varying vec2        vTexCoord;
varying vec4        vRGBA;

void main()
  {
  gl_FragColor = texture2D(sTex, vTexCoord) * vRGBA;
  }