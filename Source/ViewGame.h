#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#define MAX_PLAYERS 2
//#define INPUT_PRECALC_TEST

class Planet;
class Spaceship;
class Projectile;
class StarfieldGenerator;
class GameView : public View
	{
	public:
		class LoaderFunctor
			{
			private:
				static const Uint32		mc_uiMsgSize = 256;
				Mutex					m_Mutex;
				TXChar					m_szMessage[mc_uiMsgSize];

			public:
				LoaderFunctor();

				void SetMessage(const TXChar* pszMessage);
				void GetMessage(TXChar* pszMessage, Uint32 uiSize);
			};

	private:
		Camera					m_cam;
		Camera					m_camHUD;
		ShaderRef				m_2DShader;
		ShaderRef				m_VCShader;
#ifdef INPUT_PRECALC_TEST
		ShaderRef				m_VCShader;
#endif
		
		Planet*					m_pPlanets;
		Uint32					m_uiNumPlanets;
	
		StarfieldGenerator*		m_bg;
		PVRTVec4				m_vLight;

		Float32					m_fViewCoordX;
		Float32					m_fViewCoordY;

		PVRTVec3				m_vTouchLast;

		PVRTVec3				m_vSpringFrom;
		PVRTVec3				m_vSpringFromLast;
		bool					m_bSpringBack;
		Float32					m_fSpringPerc;

		Spaceship*				m_pSpaceships;
		Sint32					m_nPlayerTurn;
		TextureRef				m_texPlayerIndicator;
		Float32					m_fPlayerIndTime;

		TextureRef				m_texKeyboardUp;
		TextureRef				m_texKeyboardDown;
		Uint16					m_ui16KeyMask;
		Interpolator<Float32>	m_intKeyboard;

		FontRef					m_FontBig;
		FontRef					m_FontSmall;

		GameData				m_GameData;
		Float32					m_fGameEndTime;

		// AI
		Float32					m_fAILastResults[2];
		Float32					m_fAIAngleToHuman;

		// Human
		Float32					m_fEntryBeginTime;
		std::vector<Float32>	m_afTimesPerTurn[MAX_PLAYERS];

		// Audio
		AudioRef				m_Music;
		AudioRef				m_InputSfx;
		Interpolator<Float32>	m_intMusicFade;

#ifdef INPUT_PRECALC_TEST
		PrimitiveList<PrimTriangle>		m_debugtris;
		Uint32							m_debugcount;
#endif

		enum enumMESSAGELINE
			{
			enumMESSAGELINE_1,
			enumMESSAGELINE_2,
			enumMESSAGELINE_MAX,
			};

		struct SMessage
			{
			bool				bDisplay;
			Float32				fCurrUpTime;
			Float32				fMaxUpTime;
			Float32				fCharDelta;			// The step between each character been rendered
			Float32				fCharTick;
			char				szMessage[64];
			char				szMsgToRender[64];
			Uint32				uiMsgLen;
			Uint32				uiDisplayCount;
			bool				bCaret;
			bool				bInsertion;

			SMessage();
			void Set(const char* c_pszMsg, Float32 fUpTime, bool bShowCaret, bool bShowInsertionMarker);
			void Append(const char* c_pszMsg);
			void RemoveLast();
			void Update(Float32 fDT);
			}					m_Message[enumMESSAGELINE_MAX];

		class PlayerEntry
			{
			public:
				Float32				fAngle;
				Float32				fPower;

				char				szCurrent[32];

				PlayerEntry() { Reset(); }
				void Reset()  { fAngle = fPower = -1.0f; szCurrent[0] = 0; }
				PlayerEntry& operator=(const PlayerEntry& rhs)
					{
					if(&rhs != this)
						{
						fAngle = rhs.fAngle;
						fPower = rhs.fPower;
						strcpy(szCurrent, rhs.szCurrent);
						}
					return *this;
					}
			};

		PlayerEntry				m_Entry;
		PlayerEntry				m_LastEntry[enumPLAYER_MAX];

		enum enumSTATE
			{
			enumSTATE_Invalid,
			enumSTATE_PlayerInput,
			enumSTATE_TurnInProgress,
			enumSTATE_RoundFinished,
			}					m_eState;

		enum enumACTION
			{
			enumACTION_Invalid,
			enumACTION_CalcNextTurn,
			enumACTION_PlayerLaunchedProjectile,
			enumACTION_RoundFinished,
			};

		bool FindPosition(PVRTVec3& vPosOut, Float32 fRadius, Float32 fWeight = 1.0f, PVRTVec3* vHint = NULL);
		void RenderHUD();
		Sint32 TouchedKeyboard(const Touch& c_Touch);
		void InputComplete(Float32 fInput);
		void TweenKeyboard(bool bOn);

		bool CalcAI(PlayerEntry& entry);

		Float32 SimulatePhysics(PVRTVec3& vVel);

	public:
		GameView();
		~GameView();

		bool GenerateGame(LoaderFunctor& loaderProgress, const GameData& data);
		void Initialise();
		void SetState(enumACTION eAction, void* pData = NULL);

		virtual void Render();
		virtual void Update(double fDT);
		virtual void OnForeground();

		virtual void OnTouchDown(Touch* pTouches, Uint32 uiNum);
		virtual void OnTouchMoved(Touch* pTouches, Uint32 uiNum);
		virtual void OnTouchUp(Touch* pTouches, Uint32 uiNum);
	};

#endif
