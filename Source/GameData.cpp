#include "stdafx.h"

const TXChar* GameData_RoundTable[] = 
	{
	"1",
	"3",
	"5",
	"7",
	"9",
	"11",
	};

/*!***********************************************************************
 @Function		DifficultyToString
 @Access		public 
 @Param			enumGWDIFFICULTY eDiff
 @Returns		enumSTRING
 @Description	
*************************************************************************/
enumSTRING DifficultyToString(enumGWDIFFICULTY eDiff)
	{
	switch(eDiff)
		{
		case enumGWDIFFICULTY_Easy:		return enumSTRING_Easy;
		case enumGWDIFFICULTY_Medium:	return enumSTRING_Medium;
		case enumGWDIFFICULTY_Hard:		return enumSTRING_Hard;
		default:
			ASSERT(!"Unhandled difficulty!");
		}
	return DifficultyToString(enumGWDIFFICULTY_Default);
	}

/*!***********************************************************************
 @Function		StringToDifficulty
 @Access		public 
 @Param			enumSTRING eString
 @Returns		enumGWDIFFICULTY
 @Description	
*************************************************************************/
enumGWDIFFICULTY StringToDifficulty(enumSTRING eString)
	{
	switch(eString)
		{
		case enumSTRING_Easy:	return enumGWDIFFICULTY_Easy;
		case enumSTRING_Medium:	return enumGWDIFFICULTY_Medium;
		case enumSTRING_Hard:	return enumGWDIFFICULTY_Hard;
		default:
			ASSERT(!"Unhandled difficulty!");
		}
	return enumGWDIFFICULTY_Default;
	}

/*!***********************************************************************
 @Function		GameTypeToString
 @Access		public 
 @Param			enumGWGAMETYPE eType
 @Returns		enumSTRING
 @Description	
*************************************************************************/
enumSTRING GameTypeToString(enumGWGAMETYPE eType)
{
	switch(eType)
	{
		case enumGWGAMETYPE_Score:		return enumSTRING_ScoreBased;
		case enumGWGAMETYPE_Rounds:		return enumSTRING_RoundBased;
		default:
			ASSERT(!"Unhandled Game Type!");
	}
	return GameTypeToString(enumGWGAMETYPE_Default);
}

/*!***********************************************************************
 @Function		StringToGameType
 @Access		public 
 @Param			enumSTRING eString
 @Returns		enumGWGAMETYPE
 @Description	
*************************************************************************/
enumGWGAMETYPE StringToGameType(enumSTRING eString)
{
	switch(eString)
	{
		case enumSTRING_ScoreBased:	return enumGWGAMETYPE_Score;
		case enumSTRING_RoundBased:	return enumGWGAMETYPE_Rounds;
		default:
			ASSERT(!"Unhandled Game Type!");
	}
	return enumGWGAMETYPE_Default;
}

/*!***********************************************************************
 @Function		InputTypeToString
 @Access		public
 @Param			enumGWINPUTTYPE eType
 @Returns		enumSTRING
 @Description
 *************************************************************************/
enumSTRING InputTypeToString(enumGWINPUTTYPE eType)
{
	switch(eType)
	{
		case enumGWINPUTTYPE_Classic:	return enumSTRING_Classic;
		case enumGWINPUTTYPE_Touch:		return enumSTRING_Touch;
		default:
			ASSERT(!"Unhandled Input Type!");
	}
	return InputTypeToString(enumGWINPUTTYPE_Default);
}

/*!***********************************************************************
 @Function		StringToInputType
 @Access		public
 @Param			enumSTRING eString
 @Returns		enumGWINPUTTYPE
 @Description
 *************************************************************************/
enumGWINPUTTYPE StringToInputType(enumSTRING eString)
{
	switch(eString)
	{
		case enumSTRING_Classic:	return enumGWINPUTTYPE_Classic;
		case enumSTRING_Touch:		return enumGWINPUTTYPE_Touch;
		default:
			ASSERT(!"Unhandled Input Type!");
	}
	return enumGWINPUTTYPE_Default;
}

/*!***********************************************************************
 @Function		RoundsToCString
 @Access		public 
 @Param			Uint32 uiRounds
 @Returns		const TXChar*
 @Description	
*************************************************************************/
const TXChar* RoundsToCString(Uint32 uiRounds)
	{
	ASSERT(uiRounds > 0 && uiRounds <= 11);
	Uint32 uiIdx = (Uint32)(floorf(uiRounds*0.5f));		// Most implementations would floor automatically, but one never knows...!

	return GameData_RoundTable[uiIdx];
	}