#ifndef STARFIELDGENERATOR_H
#define STARFIELDGENERATOR_H

class StarfieldGenerator
	{
	public:
		enum 
			{
			TRANSRETURN_OK		= 0,
			TRANSRETURN_NOX		= 1,
			TRANSRETURN_NOY		= 2,
			};

	private:
		Bitmap		m_Bitmap;
		Uint32		m_uiTexHandle;
		Rectanglef	m_rectLoc;

		Float32		m_fHW;
		Float32		m_fHH;

		PVRTMat4	m_mxView;
		PVRTMat4	m_mxProj;
		ShaderRef	m_shader;

		void GeneratePerlin(Uint32 uiW, Uint32 uiH, Bitmap& bmPerlin);
		void GenerateGaussianNoise(Uint32 uiW, Uint32 uiH, Bitmap& bmBase, const Bitmap& bmPerlin);
		void BlitDSO(Uint32 uiW, Uint32 uiH, Bitmap& bmBase);
		void BlitStars(Uint32 uiW, Uint32 uiH, Bitmap& bmBase);

	public:
		StarfieldGenerator();
		~StarfieldGenerator();

		void Generate(Uint32 uiWidth, Uint32 uiHeight, GameView::LoaderFunctor& loaderProgress);
		void GenerateTexture();
		void Render();

		Uint32 TranslateView(const PVRTVec3& c_vTrans);
		void   SetView(const PVRTVec3& c_vTrans);
	};


#endif
