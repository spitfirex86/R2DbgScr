#include "DBGTXT.h"

MTH_tdxReal *R2_GLI_fZValueForSprite = 0x4A07D4;
BYTE *R2_GLI_bForceAAAColor = 0x4A72AC;

//
// Private
//

#define TEXT_MARGIN 2
#define FRAME_MARGIN (TEXT_MARGIN*0.1f)

JFFTXT_tdstString DBGTXT_stDummy = { NULL, 0, 0, 0, 0xFF, 0 };
void *DBGTXT_pContext = NULL;


int DBGTXT_fn_lGetCharHeight( void )
{
	float size = 15.0f - DBGTXT_stDummy.xSize;
	float height = 38.0f - size * 2.5f;
	return (int)height + TEXT_MARGIN + TEXT_MARGIN;
}

int DBGTXT_fn_lGetCharWidth( void )
{
	float size = 15.0f - DBGTXT_stDummy.xSize;
	float width = 46.0f - size * 4.0f;
	return (int)width;
}

int DBGTXT_fn_lGetFmtStringLength( const char *szFmt, va_list args )
{
	int lSize = vsnprintf(NULL, 0, szFmt, args);
	return lSize + 1;
}

//
// Public
//

BOOL DBGTXT_g_bDrawFrame = FALSE;
BOOL DBGTXT_g_bRightAlign = FALSE;


void DBGTXT_vInit( void *pContext )
{
	DBGTXT_pContext = pContext;
	DBGTXT_g_bDrawFrame = FALSE;
	DBGTXT_g_bRightAlign = FALSE;
}

void DBGTXT_vSetPos( MTH_tdxReal x, MTH_tdxReal y )
{
	DBGTXT_stDummy.X = x;
	DBGTXT_stDummy.Y = y;
}

void DBGTXT_vAddToPos( MTH_tdxReal x, MTH_tdxReal y )
{
	DBGTXT_stDummy.X += x;
	DBGTXT_stDummy.Y += y;
}

void DBGTXT_vSetSize( MTH_tdxReal xSize )
{
	DBGTXT_stDummy.xSize = xSize;
}

void DBGTXT_vPrint( char *szText )
{
	DBGTXT_stDummy.szText = szText;
	float xSaveX = DBGTXT_stDummy.X;

	MTH2D_tdstVector stTL = { 0 };
	MTH2D_tdstVector stBR = { 0 };

	if ( DBGTXT_g_bRightAlign )
	{
		JFFTXT_vGetStringExtents(&DBGTXT_stDummy, &stTL, &stBR);
		float xLength = stBR.x - stTL.x - 2 + FRAME_MARGIN + FRAME_MARGIN;

		xSaveX = DBGTXT_stDummy.X;
		DBGTXT_stDummy.X -= xLength * 10;
	}

	if ( DBGTXT_g_bDrawFrame )
	{
		JFFTXT_vGetStringExtents(&DBGTXT_stDummy, &stTL, &stBR);

		stTL.x += 1 - FRAME_MARGIN;
		stBR.x -= 1 - FRAME_MARGIN;

		float xHeight = DBGTXT_fn_lGetCharHeight() * 0.1f;
		stTL.y += 1 - FRAME_MARGIN;
		stBR.y = stTL.y + xHeight;

		float xTemp = *R2_GLI_fZValueForSprite;
		*R2_GLI_fZValueForSprite = 0.998f;
		*R2_GLI_bForceAAAColor = 0;

		GLI_vDisplayFrame(&stTL, &stBR, DBGTXT_stDummy.ucAlpha * 0.5f /*0.56f*/, DBGTXT_pContext);

		*R2_GLI_fZValueForSprite = xTemp;
	}

	JFFTXT_vDrawString(DBGTXT_pContext, &DBGTXT_stDummy);

	DBGTXT_stDummy.X = xSaveX;
	DBGTXT_stDummy.szText = NULL;
}

void DBGTXT_vPrintLine( char *szText )
{
	if ( szText )
	{
		DBGTXT_vPrint(szText);
	}
	DBGTXT_stDummy.Y += DBGTXT_fn_lGetCharHeight();
}

void DBGTXT_vPrintFmtLine( const char *szFmt, ... )
{
	va_list args;
	va_start(args, szFmt);

	int lSize = DBGTXT_fn_lGetFmtStringLength(szFmt, args);
	char *szBuffer = _malloca(lSize);

	if ( szBuffer )
	{
		vsprintf(szBuffer, szFmt, args);
		DBGTXT_vPrintLine(szBuffer);

		_freea(szBuffer);
	}

	va_end(args);
}