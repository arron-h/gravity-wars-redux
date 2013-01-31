#ifndef PLAYER_H
#define PLAYER_H

enum enumPLAYER
	{
	enumPLAYER_1,
	enumPLAYER_2,
	enumPLAYER_MAX,
	};

#define PLAYER_NAME_LEN	16

#include <vector>

class Player
	{
	private:
		TXChar		m_szName[PLAYER_NAME_LEN];
		RGBA		m_rgbaCol;
		Uint32		m_uiRoundsWon;
		Sint32		m_nScore;

		static std::vector<Player*>		m_Players;

	public:
		static void CleanUp()
			{
			for(Uint32 i = 0; i < m_Players.size(); ++i)
				delete m_Players[i];
			
			m_Players.clear();
			}

		static Player* NewPlayer(const TXChar* c_pszName, const RGBA& c_rgba)
			{
			ASSERT(m_Players.size() < enumPLAYER_MAX);
			ASSERT(strlen(c_pszName) < PLAYER_NAME_LEN);

			Player* player = new Player;;
			player->m_rgbaCol = c_rgba;
			strcpy(player->m_szName, c_pszName);
			m_Players.push_back(player);
			return player;
			}

		Player() : m_uiRoundsWon(0), m_nScore(0)
			{
			}

		inline void IncrementScore(Sint32 nScore)	{ m_nScore += nScore; }
		inline void IncrementRoundsWon()			{ m_uiRoundsWon++; }
		inline Sint32 GetScore() const				{ return m_nScore; }
		inline Uint32 GetRoundsWon() const			{ return m_uiRoundsWon; }

		inline const TXChar* GetName() const		{ return m_szName; }
		inline const RGBA& GetColour() const		{ return m_rgbaCol; }
	};

#endif
