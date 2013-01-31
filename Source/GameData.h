#ifndef GAMEDATA_H
#define GAMEDATA_H

enum enumGWDIFFICULTY
	{
	enumGWDIFFICULTY_Easy,
	enumGWDIFFICULTY_Medium,
	enumGWDIFFICULTY_Hard,

	enumGWDIFFICULTY_MAX,
	enumGWDIFFICULTY_Default = enumGWDIFFICULTY_Medium,
	};

enum enumGWGAMETYPE
	{
	enumGWGAMETYPE_Score,
	enumGWGAMETYPE_Rounds,

	enumGWGAMETYPE_MAX,
	enumGWGAMETYPE_Default = enumGWGAMETYPE_Score,
	};

#define DEFAULT_NUM_ROUNDS 3
#define MAX_NUM_ROUNDS 11

#include <vector>

struct GameData
	{
	Uint32					m_uiNumPlayers;
	Uint32					m_uiRounds;
	Uint32					m_uiRoundsPlayed;
	enumGWDIFFICULTY		m_eDifficulty;
	enumGWGAMETYPE			m_eGameType;

	std::vector<Player*>	m_pPlayers;

	GameData() : m_uiNumPlayers(0), m_uiRoundsPlayed(0), m_uiRounds(DEFAULT_NUM_ROUNDS),
				 m_eDifficulty(enumGWDIFFICULTY_Default), m_eGameType(enumGWGAMETYPE_Default) {}
	};

enumSTRING DifficultyToString(enumGWDIFFICULTY eDiff);
enumGWDIFFICULTY StringToDifficulty(enumSTRING eString);

enumSTRING GameTypeToString(enumGWGAMETYPE eType);
enumGWGAMETYPE StringToGameType(enumSTRING eString);

const TXChar* RoundsToCString(Uint32 uiRounds);

#endif
