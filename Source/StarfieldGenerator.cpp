#include "stdafx.h"
#include "ViewGame.h"
#include "StarfieldGenerator.h"
#include "PerlinNoise.h"
//#include "Perlin.h"

const Float32 c_fFieldBrightness = -0.2f;
const Float32 c_fFieldContrast   = 6.0f;
const Float32 c_fNebulaConcentration = 0.025f;

const Uint32 c_uiMVPHash = HashMap_Hash("mxMVP");

float box_muller(float m, float s);

#define DSO_TEXTURES 7
#define STAR_TEXTURES 3

/*!***********************************************************************
 @Function		ApplyBrightnessContrast
 @Access		public 
 @Param			Float32 & fInput
 @Returns		void
 @Description	
*************************************************************************/
void ApplyBrightnessContrast(Float32 &fInput, Float32 fBrightness, Float32 fContrast)
	{
	fInput += fBrightness;
	if(fInput < 0.0f)
		{
		fInput = 0.0f;
		return;
		}
		
	fInput *= fContrast;
	if(fInput > 1.0f)
		fInput = 1.0f;
	}

/*!***********************************************************************
 @Function		AgeToRGBA
 @Access		public 
 @Param			Float32 fAge
 @Param			RGBA & colourOUT
 @Returns		void
 @Description	
*************************************************************************/
void AgeToRGBA(Float32 fAge, RGBA& colourOUT)
	{
	colourOUT.m_fB = 1.0f - powf(fAge, 1.5f);		// Stars a blue when young
	colourOUT.m_fR = powf(fAge, 1.5f);
	colourOUT.m_fG = 0.4f;
	}

/*!***********************************************************************
 @Function		StarfieldGenerator
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
StarfieldGenerator::StarfieldGenerator() : m_uiTexHandle(0)
	{
	m_mxView = PVRTMat4::Identity();

	GraphicsSystem* pGfx = GetApp()->GetGraphicsSystem();
	Float32 fHW, fHH;
	if(pGfx->IsRotated())
		{
		fHW = pGfx->GetDeviceHeight() * 0.5f;
		fHH = pGfx->GetDeviceWidth() * 0.5f;
		}
	else 
		{
		fHW = pGfx->GetDeviceWidth() * 0.5f;
		fHH = pGfx->GetDeviceHeight() * 0.5f;		
		}

	m_mxProj = PVRTMat4::Ortho(-fHW, fHH, fHW, -fHH, -1.0f, 1.0f, PVRTMat4::OGL, pGfx->IsRotated());
	m_shader = RESMAN->GetShader("MVP_V_T_C");	// Simple Vertex, Tex, and RGBA shader.
	}

/*!***********************************************************************
 @Function		~StarfieldGenerator
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
StarfieldGenerator::~StarfieldGenerator()
	{
	glDeleteTextures(1, &m_uiTexHandle);
	}

/*!***********************************************************************
 @Function		GeneratePerlin
 @Access		private 
 @Param			Uint32 uiW
 @Param			Uint32 uiH
 @Param			Bitmap & bmPerlin
 @Returns		void
 @Description	
*************************************************************************/
void StarfieldGenerator::GeneratePerlin(Uint32 uiW, Uint32 uiH, Bitmap& bmPerlin)
	{
	Float32 fNoise;
	RGBA rgbaNoise;
	Float32 fractx, fracty;
	Float32 fAlpha = 0.85f;

	// --- Create some Perlin noise
	bmPerlin.Create(uiW, uiH, enumFORMAT_RGB888);
	RGBA rndCol;
	rndCol.m_fR = Randomf()*fAlpha;
	rndCol.m_fG = Randomf()*fAlpha;
	rndCol.m_fB = Randomf()*fAlpha;

	// -- First Pass
	PerlinInit();
	for(Uint32 uiY = 0; uiY < uiH; uiY++)
		{
		for(Uint32 uiX = 0; uiX < uiW; uiX++)
			{
			fractx = (float)uiX / (float)uiW;
			fracty = (float)uiY / (float)uiH;
			fNoise = PerlinNoise(fractx, fracty, 8.0f, 0.5f, 4);
			fNoise = (fNoise + 1) * 0.5f;
			ApplyBrightnessContrast(fNoise, -0.6f, 3.0f);

			rgbaNoise.Set(fNoise*rndCol.m_fR, fNoise*rndCol.m_fG, fNoise*rndCol.m_fB, 1.0f);
			bmPerlin.SetPixel(uiX, uiY, rgbaNoise);
			}
		}

	rndCol.m_fR = Randomf()*fAlpha;
	rndCol.m_fG = Randomf()*fAlpha;
	rndCol.m_fB = Randomf()*fAlpha;

	// -- Second Pass
	Bitmap bmPerlin2nd;
	PerlinInit();
	bmPerlin2nd.Create(uiW, uiH, enumFORMAT_RGB888);
	for(Uint32 uiY = 0; uiY < uiH; uiY++)
		{
		for(Uint32 uiX = 0; uiX < uiW; uiX++)
			{
			fractx = (float)uiX / (float)uiW;
			fracty = (float)uiY / (float)uiH;
			fNoise = PerlinNoise(fractx, fracty, 7.0f, 0.125f, 8);
			fNoise = (fNoise + 1) * 0.5f;
			ApplyBrightnessContrast(fNoise, -0.5f, 3.0f);

			rgbaNoise.Set(fNoise*rndCol.m_fR, fNoise*rndCol.m_fG, fNoise*rndCol.m_fB, 1.0f);
			bmPerlin2nd.SetPixel(uiX, uiY, rgbaNoise);
			}
		}

	// --- Blend both together
	bmPerlin.AddLayer(bmPerlin2nd, 0, 0, Bitmap::BLEND_PinLight);
	bmPerlin2nd.Delete();
	}

/*!***********************************************************************
 @Function		GenerateGaussianNoise
 @Access		private 
 @Param			Uint32 uiW
 @Param			Uint32 uiH
 @Param			Bitmap & bmBase
 @Returns		void
 @Description	
*************************************************************************/
void StarfieldGenerator::GenerateGaussianNoise(Uint32 uiW, Uint32 uiH, Bitmap& bmBase, const Bitmap& bmPerlin)
	{
	RGBA rgbaPixCurr;
	Float32 fNoise;
	RGBA rgbaNoise;
	RGBA rgbaPerlin;		// Pixel from the perlin noise map to determine where the nebula are
	Float32 fBrightness, fContrast, fFieldMod;
	for(Uint32 uiY = 0; uiY < uiH; uiY++)
		{
		for(Uint32 uiX = 0; uiX < uiW; uiX++)
			{
			fNoise = fabs(box_muller(0.5f, 0.25f)) * 0.2f;
			rgbaNoise.Set(fNoise, fNoise, fNoise, 1.0f);
			bmBase.GetPixel(uiX, uiY, rgbaPixCurr);
			bmPerlin.GetPixel(uiX, uiY, rgbaPerlin);
			rgbaPixCurr += rgbaNoise;

			fFieldMod = 0.0f;
			if(rgbaPerlin.m_fR > 0.1f || rgbaPerlin.m_fG > 0.1f || rgbaPerlin.m_fB > 0.1f)
				fFieldMod = c_fNebulaConcentration;

			fBrightness = c_fFieldBrightness + fFieldMod;
			fContrast   = c_fFieldContrast;

			// Adjust brightness/constrast
			ApplyBrightnessContrast(rgbaPixCurr.m_fR, fBrightness, fContrast);
			ApplyBrightnessContrast(rgbaPixCurr.m_fG, fBrightness, fContrast);
			ApplyBrightnessContrast(rgbaPixCurr.m_fB, fBrightness, fContrast);

			bmBase.SetPixel(uiX, uiY, rgbaPixCurr);
			}
		}
	}

/*!***********************************************************************
 @Function		BlitDSO
 @Access		private 
 @Param			Uint32 uiW
 @Param			Uint32 uiH
 @Param			Bitmap & bmBase
 @Returns		void
 @Description	
*************************************************************************/
void StarfieldGenerator::BlitDSO(Uint32 uiW, Uint32 uiH, Bitmap& bmBase)
	{
	char szFilename[16];

	Bitmap texDSO[DSO_TEXTURES];
	for(int i = 0; i < DSO_TEXTURES; i++)
		{
		sprintf(szFilename, "dso%02d.png", i+1);
		texDSO[i].CreateFromImage(szFilename, enumIMAGETYPE_PNG);
		}

	// --- Blit on to bitmap
	Uint32 uiRandDSONum = 1 + Randomi(2);
	for(Uint32 nDSO = 0; nDSO < uiRandDSONum; ++nDSO)
		{
		// Pick a random DSO
		Uint32 uiRndDSO = Randomi(DSO_TEXTURES - 1);
		if(!texDSO[uiRndDSO].IsValid())
			{
			nDSO--;
			continue;
			}

		Uint32 uiRandX = texDSO[uiRndDSO].GetWidth()  + Randomi( uiW - texDSO[uiRndDSO].GetWidth()*2);
		Uint32 uiRandY = texDSO[uiRndDSO].GetHeight() + Randomi( uiH - texDSO[uiRndDSO].GetHeight()*2);
		bmBase.AddLayer(texDSO[uiRndDSO], uiRandX, uiRandY, Bitmap::BLEND_Screen);
		texDSO[uiRndDSO].Delete();
		}
	}

/*!***********************************************************************
 @Function		BlitStars
 @Access		private 
 @Param			Uint32 uiW
 @Param			Uint32 uiH
 @Param			Bitmap & bmBase
 @Returns		void
 @Description	
*************************************************************************/
void StarfieldGenerator::BlitStars(Uint32 uiW, Uint32 uiH, Bitmap& bmBase)
	{
	char szFilename[16];

	Bitmap bmStar[STAR_TEXTURES];
	for(int i = 0; i < STAR_TEXTURES; i++)
		{
		sprintf(szFilename, "star%d.tga", i+1);
		bmStar[i].CreateFromImage(szFilename, enumIMAGETYPE_TGA);
		bmStar[i].ToFormat(enumFORMAT_RGBA8888);
		}

	Float32 fScale = 1.0f;;
	Uint32 uiNumPerScale = GFX->GetDeviceResolution() == enumDEVRES_HVGA ? 10 : 20;
	Bitmap bmCopy;
	RGBA rgbaAgeCol;
	for(Uint32 uiSizes = 0; uiSizes < 4; uiSizes++)
		{
		for(Uint32 uiStar = 0; uiStar < uiNumPerScale; uiStar++)
			{
			// Pick a random star bitmap
			Uint32 uiRandBM = Randomi(STAR_TEXTURES);
			// Copy it and choose a random colour
			bmStar[uiRandBM].Copy(bmCopy);
			Float32 fAge = Randomf();
			AgeToRGBA(fAge, rgbaAgeCol);
			bmCopy.Colourise(rgbaAgeCol);
			m_Bitmap.AddLayer(bmCopy, Randomi(uiW), Randomi(uiH), Bitmap::BLEND_Normal);
			bmCopy.Delete();
			}

		fScale *= 0.8f;
		for(int i = 0; i < STAR_TEXTURES; i++)
			bmStar[i].Scale(fScale, fScale);
		}
	for(int i = 0; i < STAR_TEXTURES; i++)
		bmStar[i].Delete();
	}

/*!***********************************************************************
 @Function		Generate
 @Access		public 
 @Param			Uint32 uiWidth
 @Param			Uint32 uiHeight
 @Returns		void
 @Description	
*************************************************************************/
void StarfieldGenerator::Generate(Uint32 uiWidth, Uint32 uiHeight, GameView::LoaderFunctor& loaderProgress)
	{
	Uint32 uiW = MakePow2(uiWidth);
	Uint32 uiH = MakePow2(uiHeight);
	m_Bitmap.Create(uiW, uiH, enumFORMAT_RGB888);
	m_Bitmap.FillRectangle(Rectanglei(0, 0, uiW, uiH), RGBA(0,0,0,1));

	Float32 fHW = m_fHW = uiW * 0.5f;
	Float32 fHH = m_fHH = uiH * 0.5f;
	m_rectLoc.m_fX = -fHW;
	m_rectLoc.m_fY = -fHH;
	m_rectLoc.m_fW =  fHW;
	m_rectLoc.m_fH =  fHH;

	// --- Generate Perlin Noise
	Bitmap bmPerlin;
	loaderProgress.SetMessage(GWSTR(enumSTRING_ExpandingUniverse));
	GeneratePerlin(uiW, uiH, bmPerlin);

	// --- Generate some Gaussian-distributed white noise.
	loaderProgress.SetMessage(GWSTR(enumSTRING_FormingStellaClouds));
	GenerateGaussianNoise(uiW, uiH, m_Bitmap, bmPerlin);
	
	// --- Blit Perlin onto stars
	loaderProgress.SetMessage(GWSTR(enumSTRING_GeneratingStarField));
	m_Bitmap.AddLayer(bmPerlin, 0, 0, Bitmap::BLEND_Screen);
	bmPerlin.Delete();

	// --- Load a bunch of Deep Sky Objects
	loaderProgress.SetMessage(GWSTR(enumSTRING_CreatingMassiveObjects));
	BlitDSO(uiW, uiH, m_Bitmap);

	// --- Add a bunch of bigger stars of various sizes and colours.
	BlitStars(uiW, uiH, m_Bitmap);
	}

/*!***********************************************************************
 @Function		GenerateTexture
 @Access		public 
 @Returns		void
 @Description	Uploads bitmap to GL
*************************************************************************/
void StarfieldGenerator::GenerateTexture()
	{
	// Create an OpenGL texture from this.
	m_uiTexHandle = m_Bitmap.ToOpenGL(false);
	m_Bitmap.Delete();		// We can free it now
	}

/*!***********************************************************************
 @Function		Render
 @Access		public 
 @Returns		void
 @Description	Renders the background in DEVICE specific coordinates.
*************************************************************************/
void StarfieldGenerator::Render()
	{
	Shader::Use(m_shader);
	PVRTMat4 mxMVP = m_mxProj * m_mxView;
	//Uint32 uiLoc = glGetUniformLocation(m_shader->GetHandle(), "mxMVP");
	glUniformMatrix4fv(m_shader->GetUniform(c_uiMVPHash), 1, GL_FALSE, mxMVP.f);

	glBindTexture(GL_TEXTURE_2D, m_uiTexHandle);
	GetApp()->GetGraphicsSystem()->RenderQuad(m_rectLoc, PVRTMat4::Identity(), FLAG_VRT | FLAG_TEX0 | FLAG_RGB);

	Shader::Use(0);
	}

/*!***********************************************************************
 @Function		TranslateView
 @Access		public 
 @Param			const PVRTVec3 & m_vTrans
 @Returns		void
 @Description	
*************************************************************************/
Uint32 StarfieldGenerator::TranslateView(const PVRTVec3& c_vTrans)
	{
	GraphicsSystem* pGfx = GetApp()->GetGraphicsSystem();
	Float32 fXRoom = ((m_fHW * 2.0f) - pGfx->GetDeviceWidth()) * 0.5f;
	Float32 fYRoom = ((m_fHH * 2.0f) - pGfx->GetDeviceHeight()) * 0.5f;

	// Scale the translation to get a depth effect. 
	Uint32 uiReturn = TRANSRETURN_OK;
	PVRTVec3 vTrans(c_vTrans.x * 0.5f, c_vTrans.y * 0.5f, 0.0f);
	
	// Apply a scale to the view matrix
	if(vTrans.x + m_mxView.f[12] > +fXRoom || vTrans.x + m_mxView.f[12] < -fXRoom)
		{
		vTrans.x = 0.0f;			// Use last recorded 'good' val.
		uiReturn |= TRANSRETURN_NOX;
		}
	if(vTrans.y + m_mxView.f[13] > +fYRoom || vTrans.y + m_mxView.f[13] < -fYRoom)
		{
		vTrans.y = 0.0f;			// Use last recorded 'good' val.
		uiReturn |= TRANSRETURN_NOY;
		}		

	m_mxView.f[12] += vTrans.x;
	m_mxView.f[13] += vTrans.y;

	// Make sure the coords don't go sub-pixel.
	m_mxView.f[12] = FloatRound(m_mxView.f[12]);		
	m_mxView.f[13] = FloatRound(m_mxView.f[13]);

	return uiReturn;
	}

/*!***********************************************************************
 @Function		SetView
 @Access		public 
 @Param			const PVRTVec3 & c_vTrans
 @Returns		void
 @Description	
*************************************************************************/
void StarfieldGenerator::SetView(const PVRTVec3& c_vTrans)
	{
	PVRTVec3 vTrans(c_vTrans.x * 0.5f, c_vTrans.y * 0.5f, 0.0f);

	m_mxView.f[12] = vTrans.x;
	m_mxView.f[13] = vTrans.y;

	// Make sure the coords don't go sub-pixel.
	m_mxView.f[12] = FloatRound(m_mxView.f[12]);		
	m_mxView.f[13] = FloatRound(m_mxView.f[13]);
	}

/*!***********************************************************************
 @Function		box_muller
 @Access		public 
 @Param			float m
 @Param			float s
 @Returns		float
 @Description	Code from: http://www.taygeta.com/random/boxmuller.html
*************************************************************************/
float box_muller(float m, float s)	/* normal random variate generator */
	{				        /* mean m, standard deviation s */
	float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
		{
		y1 = y2;
		use_last = 0;
		}
	else
		{
		do {
			x1 = 2.0f * Randomf() - 1.0f;
			x2 = 2.0f * Randomf() - 1.0f;
			w = x1 * x1 + x2 * x2;
			} while ( w >= 1.0f );

		w = sqrt( (-2.0f * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
		}

	return( m + y1 * s );
	}
