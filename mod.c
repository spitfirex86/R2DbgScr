#include "mod.h"
#include "script.h"
#include "DBGTXT.h"

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

void MOD_vDrawFullDebug( void *pContext )
{
	DBGTXT_vInit(pContext);
	DBGTXT_vSetSize(g_lSize);
	DBGTXT_vSetPos(g_lPosX, g_lPosY);
	DBGTXT_g_bDrawFrame = TRUE;

	// Engine
	DBGTXT_vPrintLine(TF_RED("ENGINE"));
	DBGTXT_vPrintFmtLine("state=:" TF_YELLOW("%d"), GAM_fn_ucGetEngineMode());
	DBGTXT_vPrintFmtLine("level=:" TF_YELLOW("%s"), GAM_fn_p_szGetLevelName());
	DBGTXT_vPrintLine(NULL);

	// Object stats
	g_nOnScreen = 0;
	int nActive = XHIE_fn_lEnumSpoChildren(*XHIE_p_p_stActiveDynamicWorld, MOD_bCountOnScreenCallback);
	int nInactive = XHIE_fn_lEnumSpoChildren(*XHIE_p_p_stInactiveDynamicWorld, MOD_bDummyCallback);
	int nSectors = XHIE_fn_lEnumSpoChildren(*XHIE_p_p_stFatherSector, MOD_bDummyCallback);

	DBGTXT_vPrintLine(TF_RED("OBJECT STATS"));
	DBGTXT_vPrintFmtLine(
		"active=:" TF_YELLOW("%d") "::inactive=:" TF_YELLOW("%d") "::sectors=:" TF_YELLOW("%d"),
		nActive, nInactive, nSectors
	);
	DBGTXT_vPrintFmtLine("on screen=:" TF_YELLOW("%d"), g_nOnScreen);
	DBGTXT_vPrintLine(NULL);

	// Main actor
	HIE_tdstSuperObject *pMainActor = XHIE_fn_p_stGetMainActor();
	char *szActorName = MOD_fn_szGetActorName(pMainActor);

	DBGTXT_vPrintLine(TF_RED("MAIN ACTOR"));
	DBGTXT_vPrintFmtLine("object=:" TF_YELLOW("%s") " = %p", szActorName, pMainActor);

	if ( pMainActor )
	{
		// Hitpoints
		HIE_tdstStandardGame *pStdGame = pMainActor->stEngineObject.p_stPerso->p_stStdGame;
		DBGTXT_vPrintFmtLine("hp=:" TF_YELLOW("%d ; %d"), pStdGame->ucHitPoints, pStdGame->ucHitPointsMax);
		DBGTXT_vPrintLine(NULL);

		// Coordinates
		MTH3D_tdstVector *pCoordinates = &pMainActor->p_stGlobalMatrix->stPos;
		DBGTXT_vPrintFmtLine(
			"X=:" TF_YELLOW("%.3f") "::Y=:" TF_YELLOW("%.3f") "::Z=:" TF_YELLOW("%.3f"),
			pCoordinates->x, pCoordinates->y, pCoordinates->z
		);
		DBGTXT_vPrintLine(NULL);

		// Speed
		DNM_tdstDynam *pDynam = pMainActor->stEngineObject.p_stPerso->p_stDynam;
		if ( pDynam )
		{
			MTH3D_tdstVector stSpeed = pDynam->p_stDynamics->stDynamicsBase.p_stReport->stAbsoluteCurrSpeed.stLinear;
			float sumSqXY = (stSpeed.x * stSpeed.x) + (stSpeed.y * stSpeed.y);
			float xSpeedXY = sqrtf(sumSqXY);
			float xSpeedNorm = sqrtf(sumSqXY + (stSpeed.z * stSpeed.z));

			DBGTXT_vPrintLine(TF_RED("VELOCITY"));
			DBGTXT_vPrintFmtLine("XY::speed=:" TF_YELLOW("%.3f"), xSpeedXY);
			DBGTXT_vPrintFmtLine("XYZ:speed=:" TF_YELLOW("%.3f"), xSpeedNorm);
			DBGTXT_vPrintLine(NULL);
			DBGTXT_vPrintFmtLine(
				"X=:" TF_YELLOW("%.3f") "::Y=:" TF_YELLOW("%.3f") "::Z=:" TF_YELLOW("%.3f"),
				stSpeed.x, stSpeed.y, stSpeed.z
			);
			DBGTXT_vPrintLine(NULL);
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

		DBGTXT_vPrintLine(TF_RED("TARGETING"));
		DBGTXT_vPrintFmtLine("object=:" TF_YELLOW("%s") " = %p", szTargetName, pTargetSpo);

		if ( pTargetPerso )
		{
			HIE_tdstStandardGame *pTargetStd = pTargetPerso->p_stStdGame;
			DBGTXT_vPrintFmtLine("hp=:" TF_YELLOW("%d ; %d"), pTargetStd->ucHitPoints, pTargetStd->ucHitPointsMax);
			DBGTXT_vPrintLine(NULL);

			MTH3D_tdstVector *pTargetCoords = &pTargetSpo->p_stGlobalMatrix->stPos;
			DBGTXT_vPrintFmtLine(
				"X=:" TF_YELLOW("%.3f") "::Y=:" TF_YELLOW("%.3f") "::Z=:" TF_YELLOW("%.3f"),
				pTargetCoords->x, pTargetCoords->y, pTargetCoords->z
			);
			DBGTXT_vPrintLine(NULL);

			char *szTargetOnScreen = pTargetStd->ucMiscFlags & (1 << 5) ? "yes" : "no";
			DBGTXT_vPrintFmtLine("on screen=:" TF_YELLOW("%s"), szTargetOnScreen);
			DBGTXT_vPrintLine(NULL);
		}
	}
}

void MOD_vDrawMinimalDebug( void *pContext )
{
	DBGTXT_vInit(pContext);
	DBGTXT_vSetSize(g_lSize);
	DBGTXT_vSetPos(g_lPosX, g_lPosY);
	DBGTXT_g_bDrawFrame = TRUE;

	HIE_tdstSuperObject *pMainActor = XHIE_fn_p_stGetMainActor();

	if( pMainActor )
	{
		MTH3D_tdstVector *pCoordinates = &pMainActor->p_stGlobalMatrix->stPos;
		DBGTXT_vPrintFmtLine(
			TF_RED("pos:") "X=:" TF_YELLOW("%.3f") "::Y=:" TF_YELLOW("%.3f") "::Z=:" TF_YELLOW("%.3f"),
			pCoordinates->x, pCoordinates->y, pCoordinates->z
		);

		DNM_tdstDynam *pDynam = pMainActor->stEngineObject.p_stPerso->p_stDynam;
		if ( pDynam )
		{
			MTH3D_tdstVector stSpeed = pDynam->p_stDynamics->stDynamicsBase.p_stReport->stAbsoluteCurrSpeed.stLinear;
			float sumSqXY = (stSpeed.x * stSpeed.x) + (stSpeed.y * stSpeed.y);
			float xSpeedXY = sqrtf(sumSqXY);
			float xSpeedNorm = sqrtf(sumSqXY + (stSpeed.z * stSpeed.z));

			DBGTXT_vPrintFmtLine(
				TF_RED("vel:") "XY=:" TF_YELLOW("%.3f") "::XYZ=:" TF_YELLOW("%.3f"),
				xSpeedXY, xSpeedNorm
			);
			DBGTXT_vPrintFmtLine(
				TF_RED("vel:") "X=:" TF_YELLOW("%.3f") "::Y=:" TF_YELLOW("%.3f") "::Z=:" TF_YELLOW("%.3f"),
				stSpeed.x, stSpeed.y, stSpeed.z
			);
		}
	}
}

void MOD_vDrawPrompt( void *pContext )
{
	DBGTXT_vInit(pContext);
	DBGTXT_vSetSize(g_lSize);
	DBGTXT_vSetPos(1000-5, g_lPosY);
	DBGTXT_g_bRightAlign = TRUE;
	DBGTXT_vPrintLine("F3 - " TF_YELLOW("toggle info"));
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

void MOD_JFFTXT_vAffiche( void *pContext )
{
	MOD_vDrawPrompt(pContext);

	switch( g_eDebugMode )
	{
	case e_DM_Full:
		MOD_vDrawFullDebug(pContext);
		break;

	case e_DM_Minimal:
		MOD_vDrawMinimalDebug(pContext);
		break;
	}

	JFFTXT_vAffiche(pContext);
}
