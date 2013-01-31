attribute vec3    inVertex;
attribute vec2    inTexCoord;
attribute vec4    inRGBA;
attribute vec3    inNormal;

uniform mat4      mxVP;
uniform mat4      mxModel;
uniform vec3      vLightDir;
uniform float	  fTime;
uniform float	  fAmp;

varying vec2      vTexCoord;
varying vec4      vRGBA;

void main()
  {
  float fFreq = 15.0;

  vec4 vPos  = mxModel * vec4(inVertex, 1.0);
  if(fAmp > 0.0)
	  {
	  vec3 vDisp = inNormal;
	  float fDisp = fAmp * sin(vPos.y + fTime * fFreq);
	  fDisp += fAmp/2.0  * sin(vPos.y + fTime * fFreq);
	  vDisp.x *= fDisp;         // Displace along normal X axis
	  vPos.xyz += vDisp;
	  }

  gl_Position =  mxVP * vPos;
  
  vTexCoord = inTexCoord;
  
  // Perform some quick Per-Vertex lighting.
  vec3  vN		  = normalize(inNormal);
  float vNDotL    = max(dot(inNormal, vLightDir), 0.0);
  vec3  vColComp  = vec3(vNDotL);
  vRGBA           = vec4(vColComp, 1.0);
  }