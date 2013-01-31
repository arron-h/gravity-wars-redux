#ifndef VIEWGUI_H
#define VIEWGUI_H

class ViewGUI : public View
	{
	protected:
		enum enumGUISTATE
			{
			enumGUISTATE_On			= 0x0,
			enumGUISTATE_Off		= 0x1,
			enumGUISTATE_Off2		= 0x2,
			enumGUISTATE_Off_Done	= 0x4,

			enumGUISTATE_Off_Mask = enumGUISTATE_Off | enumGUISTATE_Off2,
			};

	protected:
		Camera							m_cam;
		Camera							m_GUICam;
		PrimitiveList<PrimTriangle>		m_Tris;

		ShaderRef						m_VCShader;
		ShaderRef						m_GUIShader;

		TextureRef						m_GalaxyBGTex;
		Float32*						m_fDists;
		PVRTVec3*						m_vPositions;
		Float32							m_fGalTexRot;
		PVRTVec3						m_vSystemCentre;

		FontRef							m_FontMenu;
		FontRef							m_FontTitle;
		FontRef							m_FontSmall;

		enumGUISTATE					m_eGUIState;
		Interpolator<Float32>			m_OffInterp;
		Interpolator<Float32>			m_OffInterpGUI;
		Interpolator<Float32>			m_OffInterpAlpha;
		PVRTMat4						m_mxCam, m_mxCamGUI, m_mxCamScale;

		struct SMessage
			{
			Float32				fCurrUpTime;
			Float32				fCharDelta;			// The step between each character been rendered
			char				szMessage[1024];
			char				szMsgToRender[1024];
			Uint32				uiMsgLen;
			Uint32				uiDisplayCount;
			bool				bInsertion;
			bool				bCaret;

			SMessage*			pNext;
			Vector2i			vPos;
			enumTEXTJUSTIFY		eJustify;
			FontRef				Font;

			SMessage() : pNext(NULL) {}

			void Set(const char* c_pszMsg, bool bInsertionMarker, bool bShowCaret, Float32 fSpeed);
			void Update(Float32 fDT);
			void Layout(FontRef pFont, const Vector2i& vPosition, enumTEXTJUSTIFY eTextJustify)
				{
				Font	 = pFont;
				vPos     = vPosition;
				eJustify = eTextJustify;
				}
			};

	protected:
		virtual void RenderText() = 0;
		Vector2i GetScreenDimensions(bool bRotate);
		void Close();

	public:
		ViewGUI();
		virtual ~ViewGUI();

		virtual void Update(double fDT);
		virtual void Render();
	};

#endif
