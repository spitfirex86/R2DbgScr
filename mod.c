#include "mod.h"
#include "script.h"


typedef enum tdeDebugMode
{
	e_DM_None,
	e_DM_Full,
	e_DM_Minimal,
	e_Nb_DM
} tdeDebugMode;


WNDPROC R2_WndProc = 0x4022D0;
HWND *R2_hWnd = 0x49F080;

int g_nOnScreen = 0;
tdeDebugMode g_eDebugMode = e_DM_None;

int g_lSize = 6;
int g_lPosX = 10;
int g_lPosY = 10;


void MOD_vRunScript( HIE_tdstSuperObject *p_stSpo, AI_tdstNodeInterpret *p_stNode, AI_tdstGetSetParam *p_stParam )
{
	while ( p_stNode->ucDepth > 0 )
	{
		p_stNode = AI_fn_p_stEvalTree(p_stSpo, p_stNode, p_stParam);
	}
}

BOOL CALLBACK MOD_bCountOnScreenCallback( HIE_tdstSuperObject *p_stSpo )
{
	HIE_tdstPerso *p_stPerso = p_stSpo->stEngineObject.p_stPerso;
	if ( p_stSpo->ulType == e_OT_Perso && p_stPerso )
	{
		if ( p_stPerso->p_stStdGame->ucMiscFlags & (1 << 5) )
		{
			g_nOnScreen++;
		}
	}

	return TRUE;
}

BOOL CALLBACK MOD_bDummyCallback( HIE_tdstSuperObject *p_stSpo )
{
	return TRUE;
}

char * MOD_fn_szGetPersoName( HIE_tdstPerso *p_stPerso )
{
	char *szName = "nothing";

	if ( p_stPerso )
	{
		szName = XHIE_fn_szGetPersoName(p_stPerso, e_OI_Instance);
		if ( !szName )
		{
			szName = "unnamed object";
		}
	}

	return szName;
}

char * MOD_fn_szGetActorName( HIE_tdstSuperObject *p_stActor )
{
	char *szName = "nobody";

	if ( p_stActor )
	{
		szName = XHIE_fn_szGetObjectName(p_stActor, e_OI_Instance);
		if( !szName )
		{
			szName = "unnamed object";
		}
	}

	return szName;
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
	SPTXT_vPrintLine(NULL);

	// Object stats
	g_nOnScreen = 0;
	int nActive = XHIE_fn_lEnumSpoChildren(*XHIE_p_p_stActiveDynamicWorld, MOD_bCountOnScreenCallback);
	int nInactive = XHIE_fn_lEnumSpoChildren(*XHIE_p_p_stInactiveDynamicWorld, MOD_bDummyCallback);
	int nSectors = XHIE_fn_lEnumSpoChildren(*XHIE_p_p_stFatherSector, MOD_bDummyCallback);

	SPTXT_vPrintLine(TXT_Red("OBJECT STATS"));
	SPTXT_vPrintFmtLine(
		"active=:" TXT_Yellow("%d") "::inactive=:" TXT_Yellow("%d") "::sectors=:" TXT_Yellow("%d"),
		nActive, nInactive, nSectors
	);
	SPTXT_vPrintFmtLine("on screen=:" TXT_Yellow("%d"), g_nOnScreen);
	SPTXT_vPrintLine(NULL);

	// Main actor
	HIE_tdstSuperObject *pMainActor = XHIE_fn_p_stGetMainActor();
	char *szActorName = MOD_fn_szGetActorName(pMainActor);

	SPTXT_vPrintLine(TXT_Red("MAIN ACTOR"));
	SPTXT_vPrintFmtLine("object=:" TXT_Yellow("%s") " = %p", szActorName, pMainActor);

	if ( pMainActor )
	{
		// Hitpoints
		HIE_tdstStandardGame *pStdGame = pMainActor->stEngineObject.p_stPerso->p_stStdGame;
		SPTXT_vPrintFmtLine("hp=:" TXT_Yellow("%d ; %d"), pStdGame->ucHitPoints, pStdGame->ucHitPointsMax);
		SPTXT_vPrintLine(NULL);

		// Coordinates
		MTH3D_tdstVector *pCoordinates = &pMainActor->p_stGlobalMatrix->stPos;
		SPTXT_vPrintFmtLine(
			"X=:" TXT_Yellow("%.3f") "::Y=:" TXT_Yellow("%.3f") "::Z=:" TXT_Yellow("%.3f"),
			pCoordinates->x, pCoordinates->y, pCoordinates->z
		);
		SPTXT_vPrintLine(NULL);

		// Speed
		DNM_tdstDynam *pDynam = pMainActor->stEngineObject.p_stPerso->p_stDynam;
		if ( pDynam )
		{
			MTH3D_tdstVector stSpeed = pDynam->p_stDynamics->stDynamicsBase.p_stReport->stAbsoluteCurrSpeed.stLinear;
			float sumSqXY = (stSpeed.x * stSpeed.x) + (stSpeed.y * stSpeed.y);
			float xSpeedXY = sqrtf(sumSqXY);
			float xSpeedNorm = sqrtf(sumSqXY + (stSpeed.z * stSpeed.z));

			SPTXT_vPrintLine(TXT_Red("VELOCITY"));
			SPTXT_vPrintFmtLine("XY::speed=:" TXT_Yellow("%.3f"), xSpeedXY);
			SPTXT_vPrintFmtLine("XYZ:speed=:" TXT_Yellow("%.3f"), xSpeedNorm);
			SPTXT_vPrintLine(NULL);
			SPTXT_vPrintFmtLine(
				"X=:" TXT_Yellow("%.3f") "::Y=:" TXT_Yellow("%.3f") "::Z=:" TXT_Yellow("%.3f"),
				stSpeed.x, stSpeed.y, stSpeed.z
			);
			SPTXT_vPrintLine(NULL);
		}

		// Targeting
		SCRIPT(GetTargetPerso)
		{
			FUNC(SF_CibleLaPlusProcheavecAngles)(INT(32), INT(20), INT(2), INT(1), _FUNC(SF_GetPersoSighting), REAL(35.0f), REAL(35.0f)),
			END()
		};
		AI_tdstGetSetParam stParam = { 0 };
		MOD_vRunScript(pMainActor, GetTargetPerso, &stParam);
		HIE_tdstPerso *pTargetPerso = stParam.pValue;

		HIE_tdstSuperObject *pTargetSpo = pTargetPerso ? pTargetPerso->p_stStdGame->p_stSuperObject : NULL;
		char *szTargetName = MOD_fn_szGetPersoName(pTargetPerso);

		SPTXT_vPrintLine(TXT_Red("TARGETING"));
		SPTXT_vPrintFmtLine("object=:" TXT_Yellow("%s") " = %p", szTargetName, pTargetSpo);

		if ( pTargetPerso )
		{
			HIE_tdstStandardGame *pTargetStd = pTargetPerso->p_stStdGame;
			SPTXT_vPrintFmtLine("hp=:" TXT_Yellow("%d ; %d"), pTargetStd->ucHitPoints, pTargetStd->ucHitPointsMax);
			SPTXT_vPrintLine(NULL);

			MTH3D_tdstVector *pTargetCoords = &pTargetSpo->p_stGlobalMatrix->stPos;
			SPTXT_vPrintFmtLine(
				"X=:" TXT_Yellow("%.3f") "::Y=:" TXT_Yellow("%.3f") "::Z=:" TXT_Yellow("%.3f"),
				pTargetCoords->x, pTargetCoords->y, pTargetCoords->z
			);
			SPTXT_vPrintLine(NULL);

			char *szTargetOnScreen = pTargetStd->ucMiscFlags & (1 << 5) ? "yes" : "no";
			SPTXT_vPrintFmtLine("on screen=:" TXT_Yellow("%s"), szTargetOnScreen);
			SPTXT_vPrintLine(NULL);
		}
	}
}

void MOD_vDrawMinimalDebug( SPTXT_tdstTextInfo *pInfo )
{
	pInfo->xSize = g_lSize;
	pInfo->X = g_lPosX;
	pInfo->Y = g_lPosY;
	pInfo->bFrame = TRUE;

	HIE_tdstSuperObject *pMainActor = XHIE_fn_p_stGetMainActor();

	if( pMainActor )
	{
		MTH3D_tdstVector *pCoordinates = &pMainActor->p_stGlobalMatrix->stPos;
		SPTXT_vPrintFmtLine(
			TXT_Red("pos:") "X=:" TXT_Yellow("%.3f") "::Y=:" TXT_Yellow("%.3f") "::Z=:" TXT_Yellow("%.3f"),
			pCoordinates->x, pCoordinates->y, pCoordinates->z
		);

		DNM_tdstDynam *pDynam = pMainActor->stEngineObject.p_stPerso->p_stDynam;
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
	if ( uMsg == WM_KEYDOWN )
	{
		if ( wParam == VK_F3 )
		{
			g_eDebugMode = ++g_eDebugMode % e_Nb_DM;
			return 0;
		}
	}

	return R2_WndProc(hWnd, uMsg, wParam, lParam);
}

void MOD_Main( void )
{
	SPTXT_vInit();
	SPTXT_vAddTextCallback(MOD_vTextCallback);
}
