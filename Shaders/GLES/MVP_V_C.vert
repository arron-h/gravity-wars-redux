attribute vec3    inVertex;
attribute vec2    inTexCoord;
attribute vec4    inRGBA;

uniform mat4      mxMVP;

varying vec4      vRGBA;

void main()
  {
  gl_Position = mxMVP * vec4(inVertex, 1.0);
  vRGBA     = inRGBA;
  }