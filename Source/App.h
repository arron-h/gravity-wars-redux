#ifndef APP_H
#define APP_H

#define VIEWMAN GetApp()->GetViewManager()
#define RESMAN GetApp()->GetResourceManager()
#define AUDENG GetApp()->GetAudioEngine()

class App
	{
	protected:
		GraphicsSystem*		m_pGraphicsSys;
		ViewManager*		m_pViewMan;
		ResourceManager*	m_pResMan;
		AudioEngine*		m_pAudioEng;

		double				m_fLastTick;
		bool				m_bResLoaded;

		enumLANGUAGE		m_eLanguage;

		enum enumSTATE
		{
			enumSTATE_Initialising,
			enumSTATE_ResourceLoading,
			enumSTATE_Rendering,
		} m_eState;

    public:
        enum enumKEY
        {
            enumKEY_Back,
        };

	public:
		App();
		virtual ~App();

		GraphicsSystem* GetGraphicsSystem() { return m_pGraphicsSys; }
		ViewManager*	GetViewManager()    { return m_pViewMan; }
		ResourceManager* GetResourceManager() { return m_pResMan; }
		AudioEngine* GetAudioEngine() { return m_pAudioEng; }

		enumLANGUAGE GetLanguage() { return m_eLanguage; }
		void ToggleNextLanguage();

		// --- Implementation methods
		virtual bool Initialise(Uint32 uiWidth, Uint32 uiHeight, const char* c_pszWindowTitle) = 0;
		virtual void Destroy() = 0;
		virtual double GetTicks() = 0;


		void ResourceLoader();

		// --- Callbacks
		void OnInitialise(Float32& fDesiredVW, Float32& fDesiredVH);
		void OnViewInitialised();
		void OnUpdate();
		void OnRender();
		void OnDestroy();

		void OnTouchDown(Touch* pTouches, Uint32 uiNum);
		void OnTouchMoved(Touch* pTouches, Uint32 uiNum);
		void OnTouchUp(Touch* pTouches, Uint32 uiNum);
        void OnKeyPress(enumKEY eKey);

		void OnKeyUp(Uint32 uiKeyCode);
	};

#endif
