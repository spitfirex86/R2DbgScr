#include "mod.h"
#include "script.h"
#include "comport.h"


typedef enum tdeDebugMode
{
	e_DM_None,
	e_DM_Full,
	e_DM_FullAI,
	e_DM_Minimal,
	e_Nb_DM
} tdeDebugMode;


tdeDebugMode g_eDebugMode = e_DM_None;

int g_lSize = 6;
int g_lPosX = 10;
int g_lPosY = 10;


char const g_szNobody[] = "nobody";
char const g_szUnnamed[] = "unnamed object";
char const g_szNone[] = "none";


void MOD_vRunScript( HIE_tdstSuperObject *p_stSpo, AI_tdstNodeInterpret *p_stNode, AI_tdstGetSetParam *p_stParam )
{
	while ( p_stNode->eType != AI_E_ti_EndTree )
		p_stNode = AI_fn_p_stEvalTree(p_stSpo, p_stNode, p_stParam);
}

int MOD_fn_lCountOnScreenObjects( HIE_tdstSuperObject *p_stWorld )
{
	int lOnScreen = 0;
	HIE_tdstSuperObject *p_stSpo;

	LST_M_DynamicForEach(p_stWorld, p_stSpo)
	{
		if ( HIE_M_bSuperObjectIsActor(p_stSpo)
			&& (HIE_M_hSuperObjectGetStdGame(p_stSpo)->ucMiscFlags & Std_C_MiscFlag_UselessCulling) )
			++lOnScreen;
	}

	return lOnScreen;
}

char const * MOD_fn_szGetSuperObjName( HIE_tdstSuperObject *p_stActor )
{
	if ( p_stActor )
	{
		char *szName = HIE_fn_szGetObjectPersonalName(p_stActor);
		return (szName) ? szName : g_szUnnamed;
	}

	return g_szNobody;
}

char const * MOD_fn_szGetActorName( HIE_tdstEngineObject *p_stPerso )
{
	return (p_stPerso)
		? MOD_fn_szGetSuperObjName(HIE_M_hActorGetSuperObject(p_stPerso))
		: g_szNobody;
}

int MOD_fn_lGetCurrentComportIdx( AI_tdstIntelligence *p_stIntel )
{
	if ( !p_stIntel )
		return -1;

	AI_tdstScriptAI *p_stScriptAI = *p_stIntel->pp_stScriptAI;
	AI_tdstComport *p_stComport = p_stIntel->p_stCurrentComport;

	for ( int i = 0; i < p_stScriptAI->ulNbComport; ++i )
	{
		if ( &p_stScriptAI->a_stComport[i] == p_stComport )
			return i;
	}

	return -1;
}

void MOD_fn_vPrintComportInfo( SPTXT_tdstTextInfo *pInfo, HIE_tdstEngineObject *p_stActor )
{
	AI_tdstMind *pMind = AI_M_hGetMindOfActor(p_stActor);

	int lIdxComport = MOD_fn_lGetCurrentComportIdx(pMind->p_stIntelligence);
	int lIdxReflex = MOD_fn_lGetCurrentComportIdx(pMind->p_stReflex);

	tdstNameTable *pTable = fn_p_stGetNameTableForActor(p_stActor);
	char const *szComportName = (lIdxComport != -1) ? fn_szGetComportNameInTable(pTable, lIdxComport) : g_szNone;
	char const *szReflexName = (lIdxReflex != -1) ? fn_szGetReflexNameInTable(pTable, lIdxReflex) : g_szNone;

	//SPTXT_vPrintLine(TXT_Red("AI"));
	(szComportName)
		? SPTXT_vPrintFmtLine("comport=:" TXT_Yellow("%s"), szComportName)
		: SPTXT_vPrintFmtLine("comport=:" TXT_Yellow("%d"), lIdxComport);

	(szReflexName)
		? SPTXT_vPrintFmtLine("reflex=:" TXT_Yellow("%s"), szReflexName)
		: SPTXT_vPrintFmtLine("reflex=:" TXT_Yellow("%d"), lIdxReflex);

	SPTXT_vNewLine();
}


void MOD_vDrawFullDebug( SPTXT_tdstTextInfo *pInfo )
{
	pInfo->xSize = g_lSize;
	pInfo->X = g_lPosX;
	pInfo->Y = g_lPosY;
	pInfo->bFrame = TRUE;

	// Engine
	SPTXT_vPrintLine(TXT_Red("ENGINE"));
	SPTXT_vPrintFmtLine("state=:" TXT_Yellow("%d"), GAM_fn_ucGetEngineMode());
	SPTXT_vPrintFmtLine("level=:" TXT_Yellow("%s"), GAM_fn_p_szGetLevelName());
	SPTXT_vNewLine();

	// Object stats
	int nActive = LST_M_DynamicGetNbOfElements(*GAM_g_p_stDynamicWorld);
	int nInactive = LST_M_DynamicGetNbOfElements(*GAM_g_p_stInactiveDynamicWorld);
	int nSectors = LST_M_DynamicGetNbOfElements(*GAM_g_p_stFatherSector);
	int nOnScreen = MOD_fn_lCountOnScreenObjects(*GAM_g_p_stDynamicWorld);

	SPTXT_vPrintLine(TXT_Red("OBJECT STATS"));
	SPTXT_vPrintFmtLine(
		"active=:" TXT_Yellow("%d") "::inactive=:" TXT_Yellow("%d") "::sectors=:" TXT_Yellow("%d"),
		nActive, nInactive, nSectors
	);
	SPTXT_vPrintFmtLine("on screen=:" TXT_Yellow("%d"), nOnScreen);
	SPTXT_vNewLine();

	// Main actor
	HIE_tdstSuperObject *pMain = HIE_M_hGetMainActor();
	char const *szActorName = MOD_fn_szGetSuperObjName(pMain);

	SPTXT_vPrintLine(TXT_Red("MAIN ACTOR"));
	SPTXT_vPrintFmtLine("object=:" TXT_Yellow("%s") " = %p", szActorName, pMain);

	if ( pMain )
	{
		HIE_tdstEngineObject *pMainAct = HIE_M_hSuperObjectGetActor(pMain);

		// Hitpoints
		GAM_tdstStandardGame *pStdGame = pMainAct->hStandardGame;
		SPTXT_vPrintFmtLine("hp=:" TXT_Yellow("%d ; %d"), pStdGame->ucHitPoints, pStdGame->ucHitPointsMax);
		SPTXT_vNewLine();

		if ( g_eDebugMode == e_DM_FullAI )
		{
			// Comport
			MOD_fn_vPrintComportInfo(pInfo, pMainAct);
		}

		// Coordinates
		MTH3D_tdstVector *pCoords = &pMain->p_stGlobalMatrix->stPos;
		SPTXT_vPrintFmtLine(
			"X=:" TXT_Yellow("%.3f") "::Y=:" TXT_Yellow("%.3f") "::Z=:" TXT_Yellow("%.3f"),
			pCoords->x, pCoords->y, pCoords->z
		);
		SPTXT_vNewLine();

		// Speed
		DNM_tdstDynam *pDynam = pMainAct->hDynam;
		if ( pDynam )
		{
			MTH3D_tdstVector stSpeed = pDynam->p_stDynamics->stDynamicsBase.p_stReport->stAbsoluteCurrSpeed.stLinear;
			float sumSqXY = (stSpeed.x * stSpeed.x) + (stSpeed.y * stSpeed.y);
			float xSpeedXY = sqrtf(sumSqXY);
			float xSpeedNorm = sqrtf(sumSqXY + (stSpeed.z * stSpeed.z));

			SPTXT_vPrintLine(TXT_Red("VELOCITY"));
			SPTXT_vPrintFmtLine("XY::speed=:" TXT_Yellow("%.3f"), xSpeedXY);
			SPTXT_vPrintFmtLine("XYZ:speed=:" TXT_Yellow("%.3f"), xSpeedNorm);
			SPTXT_vNewLine();
			SPTXT_vPrintFmtLine(
				"X=:" TXT_Yellow("%.3f") "::Y=:" TXT_Yellow("%.3f") "::Z=:" TXT_Yellow("%.3f"),
				stSpeed.x, stSpeed.y, stSpeed.z
			);
			SPTXT_vNewLine();
		}

		// Targeting
		SC_Script(Scr_GetTargetPerso)
		{
			ACT_CibleLaPlusProcheAvecAngles(32, 20, 2, 1, ACT_GetPersoSighting(), 35.0f, 35.0f),
			SC_EndScript()
		};
		AI_tdstGetSetParam stParam = { 0 };
		MOD_vRunScript(pMain, Scr_GetTargetPerso, &stParam);
		HIE_tdstEngineObject *pTargetPerso = stParam.uParam.hActor;

		HIE_tdstSuperObject *pTargetSpo = pTargetPerso ? pTargetPerso->hStandardGame->p_stSuperObject : NULL;
		char const *szTargetName = MOD_fn_szGetActorName(pTargetPerso);

		SPTXT_vPrintLine(TXT_Red("TARGETING"));
		SPTXT_vPrintFmtLine("object=:" TXT_Yellow("%s") " = %p", szTargetName, pTargetSpo);

		if ( pTargetPerso )
		{
			GAM_tdstStandardGame *pTargetStd = pTargetPerso->hStandardGame;
			SPTXT_vPrintFmtLine("hp=:" TXT_Yellow("%d ; %d"), pTargetStd->ucHitPoints, pTargetStd->ucHitPointsMax);
			SPTXT_vNewLine();

			if ( g_eDebugMode == e_DM_FullAI )
			{
				// Comport
				MOD_fn_vPrintComportInfo(pInfo, pTargetPerso);
			}

			MTH3D_tdstVector *pTargetCoords = &pTargetSpo->p_stGlobalMatrix->stPos;
			SPTXT_vPrintFmtLine(
				"X=:" TXT_Yellow("%.3f") "::Y=:" TXT_Yellow("%.3f") "::Z=:" TXT_Yellow("%.3f"),
				pTargetCoords->x, pTargetCoords->y, pTargetCoords->z
			);
			SPTXT_vNewLine();

			char *szTargetOnScreen = pTargetStd->ucMiscFlags & (1 << 5) ? "yes" : "no";
			SPTXT_vPrintFmtLine("on screen=:" TXT_Yellow("%s"), szTargetOnScreen);
		}
		SPTXT_vNewLine();
	}
}

void MOD_vDrawMinimalDebug( SPTXT_tdstTextInfo *pInfo )
{
	pInfo->xSize = g_lSize;
	pInfo->X = g_lPosX;
	pInfo->Y = g_lPosY;
	pInfo->bFrame = TRUE;

	HIE_tdstSuperObject *pMainActor = HIE_M_hGetMainActor();

	if( pMainActor )
	{
		MTH3D_tdstVector *pCoordinates = &pMainActor->p_stGlobalMatrix->stPos;
		SPTXT_vPrintFmtLine(
			TXT_Red("pos:") "X=:" TXT_Yellow("%.3f") "::Y=:" TXT_Yellow("%.3f") "::Z=:" TXT_Yellow("%.3f"),
			pCoordinates->x, pCoordinates->y, pCoordinates->z
		);

		DNM_tdstDynam *pDynam = pMainActor->hLinkedObject.p_stCharacter->hDynam;
		if ( pDynam )
		{
			MTH3D_tdstVector stSpeed = pDynam->p_stDynamics->stDynamicsBase.p_stReport->stAbsoluteCurrSpeed.stLinear;
			float sumSqXY = (stSpeed.x * stSpeed.x) + (stSpeed.y * stSpeed.y);
			float xSpeedXY = sqrtf(sumSqXY);
			float xSpeedNorm = sqrtf(sumSqXY + (stSpeed.z * stSpeed.z));

			SPTXT_vPrintFmtLine(
				TXT_Red("vel:") "XY=:" TXT_Yellow("%.3f") "::XYZ=:" TXT_Yellow("%.3f"),
				xSpeedXY, xSpeedNorm
			);
			SPTXT_vPrintFmtLine(
				TXT_Red("vel:") "X=:" TXT_Yellow("%.3f") "::Y=:" TXT_Yellow("%.3f") "::Z=:" TXT_Yellow("%.3f"),
				stSpeed.x, stSpeed.y, stSpeed.z
			);
		}
	}
}

void MOD_vDrawPrompt( SPTXT_tdstTextInfo *pInfo )
{
	pInfo->xSize = g_lSize;
	pInfo->X = 1000-5;
	pInfo-> Y = g_lPosY;
	pInfo->bRightAlign = TRUE;
	SPTXT_vPrintLine("F3 - " TXT_Yellow("toggle info"));
}

void CALLBACK MOD_vTextCallback( SPTXT_tdstTextInfo *p_stInfo )
{
	MOD_vDrawPrompt(p_stInfo);
	SPTXT_vResetTextInfo(p_stInfo);

	switch( g_eDebugMode )
	{
	case e_DM_Full:
	case e_DM_FullAI:
		MOD_vDrawFullDebug(p_stInfo);
		break;

	case e_DM_Minimal:
		MOD_vDrawMinimalDebug(p_stInfo);
		break;
		
	default: break;
	}
}

LRESULT CALLBACK MOD_WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( uMsg == WM_KEYDOWN && IPT_g_stInputStructure->ulNumberOfEntryElement )
	{
		if ( wParam == VK_F3 )
		{
			g_eDebugMode = ++g_eDebugMode % e_Nb_DM;
			return 0;
		}
	}

	return GAM_fn_WndProc(hWnd, uMsg, wParam, lParam);
}

void MOD_Main( void )
{
	fn_vReadComportNames();
	SPTXT_vInit();
	SPTXT_vAddTextCallback(MOD_vTextCallback);
}
