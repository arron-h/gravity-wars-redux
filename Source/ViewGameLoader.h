#ifndef GAMEVIEWLOADER_H
#define GAMEVIEWLOADER_H

class ViewGameLoader : public View
	{
	private:
		Camera					m_cam;
		FontRef					m_Font;
		ShaderRef				m_shaderFont;
		struct GameLoaderData
			{
			GameView*				pGameView;
			GameData				Data;
			GameView::LoaderFunctor ProgressFunc;
			GameLoaderData() : pGameView(NULL) {}
			}					m_LoaderData;

		AudioRef				m_MenuMusic;
		Float32_Clamp			m_fMusicFade;

	private:
		static void* LoadGame(void* pArg);

	public:
		ViewGameLoader(const GameData& gamedata);
		virtual void Render();
		virtual void Update(double dt);
	};

#endif
