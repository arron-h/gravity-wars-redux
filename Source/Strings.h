#ifndef STRINGS_H
#define STRINGS_H

enum enumSTRING
	{
	enumSTRING_Player1,
	enumSTRING_Player2,
	enumSTRING_AI,
	enumSTRING_Loading,
	enumSTRING_InputVel,
	enumSTRING_InputPwr,
	enumSTRING_LastEntry,

	enumSTRING_ExpandingUniverse,
	enumSTRING_FormingStellaClouds,
	enumSTRING_CreatingMassiveObjects,
	enumSTRING_GeneratingStarField,
	enumSTRING_PopulatingSystem,
	enumSTRING_PositingShips,

	enumSTRING_MenuItem_1Player,
	enumSTRING_MenuItem_2Player,
	enumSTRING_MenuItem_Credits,
	enumSTRING_MenuItem_Language,
	enumSTRING_MenuItem_PleaseSelect,
	enumSTRING_Title,
	enumSTRING_Copyright,
	enumSTRING_Credits,
	enumSTRING_CreditsText1,
	enumSTRING_CreditsText2,
	enumSTRING_Back,
	enumSTRING_1Player,
	enumSTRING_2Player,
	enumSTRING_NumRounds,
	enumSTRING_Difficulty,
	enumSTRING_GameType,
	enumSTRING_ScoreBased,
	enumSTRING_RoundBased,
	enumSTRING_GameTypeDesc,
	enumSTRING_PlayGame,
	enumSTRING_Classic,
	enumSTRING_Touch,
	enumSTRING_Controls,

	enumSTRING_RoundXofY,

	enumSTRING_GameOver,
	enumSTRING_XWins,
	enumSTRING_ScoreX,
	enumSTRING_BestScoreX,
	enumSTRING_RoundsWonX,
	enumSTRING_MainMenu,
	
	enumSTRING_Easy,
	enumSTRING_Medium,
	enumSTRING_Hard,
		
	enumSTRING_Player1TakeTurn,
	enumSTRING_Player2TakeTurn,

	enumSTRING_Invalid,

	enumSTRING_MAX,
	};

#ifdef __cplusplus
extern const char* GetString(enumSTRING eString);
#endif

#define GWSTR(x) GetString(x)

#endif
