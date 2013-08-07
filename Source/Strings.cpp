#include "stdafx.h"

const char* c_pszText[enumLANGUAGE_MAX][enumSTRING_MAX] =
	{	
		// English
		{
		"Player 1",
		"Player 2",
		"AI",
		"Loading...",
		"Enter Angle: ",
		"Enter Power: ",
		"Last Angle: %.0f | Last Power: %.0f",

		"Expanding Universe",
		"Forming Interstellar Clouds",
		"Creating Massive Objects",
		"Generating Starfield",
		"Populating System",
		"Positioning Spaceships",

		"> 1 Player\n\n> 2 Player\n\n> Credits\n\n> English\n\nPlease select an item",
		"Gravity Wars Redux",
		"A forGreatJustice Production. Copyright © Arron Hartley.",
		"Credits",
		"Design and code: Arron Hartley",
		"Deep sky images courtesy of STScl [NASA]\n\nPlanet textures copyright © James Hastings-Trew\n\nMusic from AD Music Online\n\nThanks to Imagination Technologies for their SDK and tools\n\n\nsupport@forgreatjustice.co.uk",
		"Back",
		"1 Player",
		"2 Player",
		"Num. Rounds:",
		"Difficulty:",
		"Game Type:",
		"Score",
		"Rounds",
		"Score-based gameplay determines the winner on their total score at the end of the game. The winner of a 'Round'-based game is purely the player with the most wins.",
		"Play Game",
		"Classic",
		"Touch",
		"Controls:",

		"[Round %d of %d]",

		"Game Over",
		"%s wins!",
		"Score: %d",
		"Best Score: %d",
		"Rounds Won: %u",
		"Main Menu",
		
		"Easy",
		"Medium",
		"Hard",
			
		"Player 1 Take Your Turn",
		"Player 2 Take Your Turn",
			
		"",
		},

		// French
		{
		"Joueur 1",
		"Joueur 2",
		"AI",
		"Chargement...",
		"Entrez Angle: ",
		"Entrez Pouvoir: ",
		"Dernier Angle: %.0f | Dernière Puissance: %.0f",

		"Expansion de l'Univers",
		"Formant des Nuages Interstellaires",
		"Création d'Objets Massifs",
		"Génération Starfield",
		"Remplissage du Système",
		"Vaisseaux Spatiaux de Positionnement",

		"> 1 Joueur\n\n> 2 Joueur\n\n> Crédits\n\n> Fran""\x00E7""ais\n\nS'il vous plaît sélectionner un élément",
		"Gravity Wars Redux",
		"Une production forGreatJustice. Copyright © 2011 forGreatJustice.",
		"Crédits",
		"Design et code: Arron Hartley",
		"Deep sky images avec la permission de STScl [NASA]\n\nTextures planet copyright © James Hastings-Trew\n\nsupport@forgreatjustice.co.uk",
		"Retour",
		"1 Joueur",
		"2 Joueur",
		"Tours:",
		"Difficulté:",
		"Type de jeu:",
		"Valuation",
		"Tours",
		"Score-based gameplay determines the winner on their total score at the end of the game. The winner of a 'Round'-based game is purely the player with the most wins.",
		"Jeu",
		"Classic",
		"Touch",
		"Controls:",

		"[Tour %d de %d]",

		"Game Over",
		"%s wins!",
		"Score: %d",
		"Meilleur Score: %d",
		"Rounds Gagnés: %u",
		"Menu Principal",

		"Facile",
		"Moyennes",
		"Dur",
			
		"Player 1 Take Your Turn",
		"Player 2 Take Your Turn",

		"",
		},
	};

const char* GetString(enumSTRING eString)
	{
	return c_pszText[GetApp()->GetLanguage()][eString];
	}